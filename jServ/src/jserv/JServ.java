package jserv;

/**
 *
 * @author cmo
 */
public class JServ {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Server s = new Server(args[0]);
        s.run();
    }
    
}
