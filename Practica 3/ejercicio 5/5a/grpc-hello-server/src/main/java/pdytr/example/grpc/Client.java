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

    String str = "A";

    GreetingServiceGrpc.GreetingServiceBlockingStub stub = GreetingServiceGrpc.newBlockingStub(channel);
    GreetingServiceOuterClass.HelloRequest request = GreetingServiceOuterClass.HelloRequest.newBuilder()
      .setName(str)
      .build();

    for (int r=0; r<100; r++) {

      Date t0 = new Date();
      GreetingServiceOuterClass.HelloResponse response = stub.greeting(request);
      Date t1 = new Date();

      System.out.println("Elapsed time: " + (((t1.getTime() - t0.getTime()) * 0.001) / 2) + " s");

    }

    // A Channel should be shutdown before stopping the process.
    channel.shutdownNow();
  }
}