package pdytr.example.grpc;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.google.protobuf.ByteString;

import io.grpc.stub.StreamObserver;

public class FileTransferServiceImpl extends FileTransferServiceGrpc.FileTransferServiceImplBase {
	
	public void finishReadRequest(int bytes_read, ByteString buffer, StreamObserver<FileTransferServiceOuterClass.ReadResponse> response_observer) {
		FileTransferServiceOuterClass.ReadResponse response = FileTransferServiceOuterClass.ReadResponse.newBuilder()
				  .setReadSize(bytes_read)
				  .setData(buffer)
				  .build();
		response_observer.onNext(response);
		response_observer.onCompleted();
	}
	
	public void finishWriteRequest(long file_size, StreamObserver<FileTransferServiceOuterClass.WriteResponse> response_observer) {
		FileTransferServiceOuterClass.WriteResponse response = FileTransferServiceOuterClass.WriteResponse.newBuilder()
				.setBytesWritten(file_size)
				.build();
		
		response_observer.onNext(response);
		response_observer.onCompleted();
	}
	
	@Override
	public void read(FileTransferServiceOuterClass.ReadRequest request,
		  StreamObserver<FileTransferServiceOuterClass.ReadResponse> responseObserver) {
		
		int bytes_read = 0;
		ByteString buffer = null;
		try {
			InputStream inStream = new FileInputStream(request.getFilename());
			byte[] data = new byte[request.getSize()];
			inStream.skip(request.getPosition());
			bytes_read = inStream.read(data, 0, request.getSize());
			buffer = ByteString.copyFrom(data);
			inStream.close();
			
		} catch(IOException e) {
			System.out.println("Error producido: " + e.getMessage());
		}
		finishReadRequest(bytes_read,buffer,responseObserver);		
  }
  
	@Override
	public void write(FileTransferServiceOuterClass.WriteRequest request,
		  StreamObserver<FileTransferServiceOuterClass.WriteResponse> response_observer) {
	
		File file = null;
		try {
			file = new File(request.getFilename());
			FileOutputStream outStream = new FileOutputStream(file, file.exists());
			outStream.write(request.getBuffer().toByteArray(), 0, request.getDataSize());
			outStream.close();	
		} catch (FileNotFoundException e) {
			System.out.println("Error producido: " + e.getMessage());
		} catch (IOException e1) {
			System.out.println("Error producido: " + e1.getMessage());
		}
		finishWriteRequest(file.length(),response_observer);
	}
}