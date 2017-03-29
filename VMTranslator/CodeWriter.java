import java.io.IOException;

// Generates assembly code from the parsed VM command.
public class CodeWriter implements AutoCloseable {
    // Opens the output file/stream and gets ready to write into it.
    public CodeWriter(String filename) {

    }

    // Writes to the output file the assembly code that implements the
    // given arithmetic command.
    public void writeArithmetic(String command) {
        System.out.println(command);
    }

    // Writes to the output file the assembly code that implements the
    // given command, where command is either C_PUSH or C_POP.
    public void writePushPop(Parser.CommandType command, String segment,
            int index) {
        String commandString = "";
        switch (command) {
            case C_PUSH:
                commandString = "push";
                break;
            case C_POP:
                commandString = "pop";
                break;
        }
        System.out.printf("%s %s %d\n", commandString, segment, index);
    }

    // Closes the output file.
    public void close() throws IOException {

    }
}
