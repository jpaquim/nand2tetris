import java.io.IOException;

//
// Constructs a Parser to handle the input file
// Constructs a CodeWriter to handle the output file
// Marches through the input file, parsing each line and generating code
// from it
public class VMTranslator {
    public static void main(String[] args) {
        // check if the command line is valid
        if (args.length != 1) {
            System.out.println("Usage: java HackVM filename");
            return;
        }
        String inFilename = args[0];
        // check if the input filename has .vm extension
        int lastDot = inFilename.lastIndexOf('.');
        if (!"vm".equals(inFilename.substring(lastDot+1))) {
            System.out.println("Error: input file must have .vm extension");
        }
        // build the output filename by appending .asm to the base name
        String outFilename = inFilename.substring(0, lastDot) + ".asm";

        try (
            Parser parser = new Parser(inFilename);
            CodeWriter codeWriter = new CodeWriter(outFilename)) {
            while (parser.hasMoreCommands()) {
                parser.advance();
                switch (parser.commandType()) {
                    case C_ARITHMETIC:
                        codeWriter.writeArithmetic(parser.arg1());
                        break;
                    case C_PUSH:
                    case C_POP:
                        codeWriter.writePushPop(parser.commandType(),
                                parser.arg1(), parser.arg2());
                        break;
                }
            }
        } catch (IOException e) {
            // TODO: handle IOException, distinguish between input and
            // output file exceptions
            System.out.println(
                    "Error: failed to open input or output file");
        }
    }
}
