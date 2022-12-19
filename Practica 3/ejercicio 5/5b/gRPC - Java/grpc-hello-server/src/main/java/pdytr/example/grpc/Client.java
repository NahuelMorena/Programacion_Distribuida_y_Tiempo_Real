package pdytr.example.grpc;

import io.grpc.*;
import com.google.protobuf.ByteString;
import java.util.Date;

public class Client
{

  public static void main( String[] args ) throws Exception
  {
    // Channel is the abstraction to connect to a service endpoint
    // Let's use plaintext communication because we don't have certs
    final ManagedChannel channel = ManagedChannelBuilder.forTarget("localhost:8080").usePlaintext(true).build();

    String str = "";
    int bytes_count = 1;

    for (int t=0; t<6; t++) {

      bytes_count = bytes_count * 10;

      //Genero un String de N bytes
      for (int i=0; i<(bytes_count-1); i++) {
        str = str + 'A';
      }

      //Convierto de tipo String a ByteString
      ByteString byteString = ByteString.copyFromUtf8(str);
  
      GreetingServiceGrpc.GreetingServiceBlockingStub stub = GreetingServiceGrpc.newBlockingStub(channel);
      GreetingServiceOuterClass.HelloRequest request = GreetingServiceOuterClass.HelloRequest.newBuilder()
        .setName(byteString)
        .build();
  
      double sum = 0.0;

      for (int r=0; r<200; r++) {
  
        Date t0 = new Date();
        GreetingServiceOuterClass.HelloResponse response = stub.greeting(request);
        Date t1 = new Date();
    
        sum = sum + (((t1.getTime() - t0.getTime()) * 0.001) / 2);

      }

      System.out.println("Mean elapsed time: " + (sum/200) + " s");
    
    }

    // A Channel should be shutdown before stopping the process.
    channel.shutdownNow();
  }
}