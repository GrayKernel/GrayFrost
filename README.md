#GrayFrost

GrayFrost is a delivery system for C# payloads. GrayFrost is outputted as a DLL that can be injected into a target application. 

GrayFrost was built by [Topher Timzen](https://tophertimzen.com) and is currently under active support. If you have any issues or pull requests, do not hesitate to submit them! 

C++ .NET CLR Bootstrapper
	
- Creates or injects 4.0 runtime
- Capability to pivot into 2.0 runtime
- Contains raw payload
	
2 Rounds

- GrayFrostCpp
- GrayFrostCSharp	
	- C# Payload



##Build:

To build GrayFrost, [autoFrost](https://bitbucket.org/tophertimzen/autofrost) is recommended. This tool will auto-bundle the two byte arrays (the payload and the GrayFrost C# round) into the C++ DLL. 

Manual Build Process: 

1. Obtain a C# byte array for the C# payload and place it in *GrayFrostCSharp\payload.cs*. 
2. Compile GrayFrostCSharp.
3. Obtain a C++ byte array for the GrayFrostCSharp executable and place it in *GrayFrost\payloading.h*. 
4. Compile GrayFrost
5. Inject GrayFrost{32,64} into target application. 

For the manual build, ensure the namespace is correct. 

