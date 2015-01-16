package jserv;

/**
 * Class: Server
 *
 * The server class creates and run the servers protocol for file transfer.
 *
 * @author Sean Morgan
 */
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;

public class Server {

    //Constants
    private static final int PORTNUM = 5976;
    private static final int CHUNK_SIZE = 1024;
    private static final int OFF_SET = 0;

    //Variables
    ServerSocket servSock;
    String ipAddress;
    FileOutputStream ff;   // Used to read the file.
    InputStream in;            // Used to read data from socket
    OutputStream out;          // Used to write data to socket
    String saveFileTo;

    /**
     * Constructor:
     *
     * Creates a server socket.
     */
    Server(String save) {
        saveFileTo = save;
        try {
            servSock = new ServerSocket(PORTNUM);
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

    /**
     * Method run():
     *
     * Starts the server and waits for a connection then it starts receiving of
     * the file from the client.
     */
    public void run() {
        System.out.println("Server running...");
        System.out.println("Server listening on port: " + servSock.getLocalPort());
        while (true) {
            try {
                System.out.println("Waiting for connection...");
                Socket sock = servSock.accept();   // listens for connections
                System.out.println("Got a connection: " + new Date().toString());
                out = sock.getOutputStream();
                in = sock.getInputStream();
                System.out.println("Connected to: " + sock.getInetAddress().getCanonicalHostName() + " Port: "
                        + sock.getPort() + '\n');
                start();
                sock.close();
                System.out.println("Connection Closed");

            } catch (IOException ex) {
                System.out.println(ex.getMessage());
            }
        }
    }

    /**
     * Method start():
     *
     * The basic protocol of uploading a file. First receives the file name then
     * the size and finally the file.
     */
    private void start() {
        String fileName = getNullTerminatedString();
        System.out.println("Got file name: " + fileName);
        String filesi = getNullTerminatedString();
        System.out.println("File size: " + filesi);
        long fileSize = Long.parseLong(filesi);
        System.out.println("Recieving File...");
        getFile(saveFileTo + fileName, fileSize);
    }

    /**
     * Method getNullTerminatedString():
     *
     * This method reads the bytes (terminated by ‘\0’) sent from the Client,
     * one byte at a time, and turns the bytes into a String.
     */
    private String getNullTerminatedString() {
        //set up a loop to repeatedly read bytes until a ‘\0’ is reached
        String t = "";
        try {
            byte buff[] = new byte[1];
            in.read(buff);
            while (buff[0] != '\0') {
                t = t.concat(new String(buff));
                in.read(buff);
            }
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
        return t;
    }

    /**
     * Method getFile(String, long):
     *
     * This method takes an output file name and its file size, reads the binary
     * data (in a 1024-byte chunk) sent from the Client, and writes to the
     * output file a chunk at a time.
     */
    private void getFile(String filename, long fileSize) {
        //use the FileOutputStream class to write bytes to a binary file
        //set up a loop to repeatedly read and write chunks
        try {
            File f = new File(filename);
            ff = new FileOutputStream(f);
            long totalRead = 0;
            byte buff[] = new byte[CHUNK_SIZE]; //CHUNK_SIZE is 1024 bytes
            while (totalRead < fileSize) {
                int numRead = in.read(buff); // try to read some bytes from the socket
                if (numRead != -1) {
                    ff.write(buff, OFF_SET, numRead);
                    totalRead += numRead;
                } else {
                    System.err.println("Problem Reading Socket...");
                }
            }
            if (totalRead == fileSize) {
                out.write('@');
                System.out.println("File successfully uploaded.");
            } else {
                out.write(' ');
                System.out.println("File failed to upload.");
            }
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
}
