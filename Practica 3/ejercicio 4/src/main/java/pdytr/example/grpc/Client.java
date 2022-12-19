package pdytr.example.grpc;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.google.protobuf.ByteString;

import io.grpc.*;

public class Client
{
	private static String root = "";
	
	public static FileTransferServiceOuterClass.ReadRequest doReadRequest(String server_file_name, int size, int pos){
		FileTransferServiceOuterClass.ReadRequest request = FileTransferServiceOuterClass.ReadRequest.newBuilder()
				.setFilename(server_file_name)
				.setSize(size)
				.setPosition(pos)
				.build();
		return request;
	}
	
	public static FileTransferServiceOuterClass.WriteRequest doWriteRequest(String destiny_root_file, int bytes_read, byte[] buffer){
		FileTransferServiceOuterClass.WriteRequest request = FileTransferServiceOuterClass.WriteRequest.newBuilder()
				.setFilename(destiny_root_file)
				.setDataSize(bytes_read)
				.setBuffer(ByteString.copyFrom(buffer))
				.build();
		return request;
	}
	
	public static Boolean verifyFileExist(String file_name) {
		File file = new File(root+file_name);
		return file.exists();
	}
	
	public static int writeNewClientFile(FileTransferServiceOuterClass.ReadResponse response, FileOutputStream out_Stream) throws IOException {
		byte[] in_Stream = null;
		int bytes_read = response.getReadSize();
		in_Stream = response.getData().toByteArray();
		out_Stream.write(in_Stream, 0, bytes_read);
		return bytes_read;
	}
	
	public static void read(FileTransferServiceGrpc.FileTransferServiceBlockingStub stub, String name_file) throws FileNotFoundException {
		root = "./RepositoryServer/";
		
		if (!verifyFileExist(name_file)) {
			throw new FileNotFoundException("The file '/"+name_file+"' does not exist in the directory "+ root);
		}
		
		String destiny_root_file = "./ClientLocalSpace/copy-"+name_file;
		String server_file_name = root + name_file;

		int buffer_size = 1000;
		int pos = 0;
		FileTransferServiceOuterClass.ReadResponse response = stub.read(doReadRequest(server_file_name,buffer_size,pos));
		try {
			FileOutputStream out_Stream = new FileOutputStream(destiny_root_file);
			while(response.getReadSize() != -1) {
				pos += writeNewClientFile(response, out_Stream);
				response = stub.read(doReadRequest(server_file_name,buffer_size,pos));
			}
			out_Stream.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Read operation completed!!");
	}
	
	public static void write(FileTransferServiceGrpc.FileTransferServiceBlockingStub stub, String name_file) throws FileNotFoundException{
		root = "./ClientLocalSpace/";
		
		if (!verifyFileExist(name_file)) {
			throw new FileNotFoundException("The file '/"+name_file+"' does not exist in the directory "+ root);
		}
		
		String destiny_root_file = "./RepositoryServer/"+name_file;
		String client_file_name = root + name_file;

		int buffer_size = 1000;
		byte[] buffer = new byte[buffer_size];
		File file = new File(client_file_name);
		long file_size = file.length();
		int bytes_read = 0;
		
		try {
			FileInputStream in_Stream = new FileInputStream(file);
			FileTransferServiceOuterClass.WriteResponse response;
			do {
				bytes_read = in_Stream.read(buffer, 0, buffer_size);
				response = stub.write(doWriteRequest(destiny_root_file, bytes_read, buffer));
			} while (response.getBytesWritten() < file_size);
			in_Stream.close();
		} catch (FileNotFoundException e) {
			throw new FileNotFoundException(e.getMessage());
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		System.out.println("Write operation completed!!");
	}
	
    public static void main( String[] args ) throws Exception {
    	
      final ManagedChannel channel = ManagedChannelBuilder.forTarget("localhost:8080")
        .usePlaintext(true)
        .build();
      
      FileTransferServiceGrpc.FileTransferServiceBlockingStub stub = FileTransferServiceGrpc.newBlockingStub(channel);
      String op = "";
            
      try {
    	  op = args[0];
    	  
    	  switch(op) {
    	  case "-r":
    		  read(stub, args[1]);
    		  break;
    	  case "-w":
    		  write(stub, args[1]);
    		  break;
    	  default:
    		  System.out.println("Error: invalid value!!");
    		  System.out.println("Valid values: Read:'-r' <---> Write:'-w'");
    	  }
      } catch (ArrayIndexOutOfBoundsException e) {
    	  System.out.println("Error: Missing arguments");
    	  System.out.println("First argument: Read: -r ---------- Write: -w"); 
    	  System.out.println("Second argumento: Name file to operate, example: file1.txt");
      } catch (FileNotFoundException e) {
    	  System.out.println(e.getMessage());
      }
    } 
}