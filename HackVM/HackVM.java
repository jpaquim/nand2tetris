public class HackVM {
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

        Parser parser = new Parser(inFilename);
        CodeWriter codeWriter = new CodeWriter(outFilename);
    }
}
