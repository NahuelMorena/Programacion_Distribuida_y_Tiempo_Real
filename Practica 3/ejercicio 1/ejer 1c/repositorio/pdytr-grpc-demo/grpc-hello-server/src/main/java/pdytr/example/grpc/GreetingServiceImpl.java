package pdytr.example.grpc;

import java.util.concurrent.TimeUnit;

import io.grpc.stub.StreamObserver;

public class GreetingServiceImpl extends GreetingServiceGrpc.GreetingServiceImplBase {
  @Override
  public void greeting(GreetingServiceOuterClass.HelloRequest request,
        StreamObserver<GreetingServiceOuterClass.HelloResponse> responseObserver) {
    // HelloRequest has toString auto-generated.
	//try {
	//	TimeUnit.MILLISECONDS.sleep(5000);
	
		System.out.println(request);

		// You must use a builder to construct a new Protobuffer object
		GreetingServiceOuterClass.HelloResponse response = GreetingServiceOuterClass.HelloResponse.newBuilder()
				.setGreeting("Hello there, " + request.getName())
				.build();
		try {
			TimeUnit.MILLISECONDS.sleep(4500);
		} catch (Exception e) {
			e.printStackTrace();
		}
    
		// Use responseObserver to send a single response back
		responseObserver.onNext(response);

		// When you are done, you must call onCompleted.
		responseObserver.onCompleted();
	//} catch (Exception e) {
	//	e.printStackTrace();
	//}
  }
}