import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;
import java.nio.file.Files;
import java.nio.file.Paths;

// Handles the parsing of a single .vm file.
// Reads a VM command, parses the command into its lexical components, and
// provides convenient access to these components.
// Ignores all white space and comments.

public class Parser implements AutoCloseable {
    
    // Enum representing the various different types of commands.
    public enum CommandType {
        C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION,
        C_RETURN, C_CALL
    }

    private BufferedReader reader;
    // private Scanner scanner;

    // Opens the input file/stream and gets ready to parse it.
    public Parser(String filename) throws IOException {
        reader = Files.newBufferedReader(Paths.get(filename));
        // reader = new BufferedReader(new FileReader(filename));
        // Scanner scanner = new Scanner(filename);
    }

    // Are there more commands in the input?
    public boolean hasMoreCommands() {
        return false;
    }

    // Reads the next command from the input and makes it the current
    // command. Should be called only if hasMoreCommands() is true.
    // Initially there is no current command.
    public void advance() {

    }

    // Returns a constant representing the type of the current command.
    // C_ARITHMETIC is returned for all the arithmetic/logical commands.
    public CommandType commandType() {
        return CommandType.C_RETURN;
    }

    // Returns the first argument of the current command. In the case of
    // C_ARITHMETIC, the command itself (add, sub, etc.) is returned.
    // Should not be called if the current command is C_RETURN.
    public String arg1() {
        return "";
    }

    // Returns the second argument of the current command. Should be called
    // only if the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL.
    public int arg2() {
        return 0;
    }

    // Closes the parser's input reader.
    public void close() throws IOException {
        reader.close();
    }
}
