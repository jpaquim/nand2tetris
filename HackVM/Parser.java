public class Parser {

    public enum CommandType {
        C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION,
        C_RETURN, C_CALL
    }

    public Parser(String filename) {

    }

    public boolean hasMoreCommands() {
        return false;
    }

    public void advance() {

    }

    public CommandType commandType() {
        return CommandType.C_RETURN;
    }

    public String arg1() {
        return "";
    }

    public int arg2() {
        return 0;
    }
}
