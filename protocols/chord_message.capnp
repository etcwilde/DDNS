@0xb505c13fe7209fc6;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("DNS");

struct Request {
	enum RequestType {
		none @0;
		join @1;
		get @2;
		sync @3;
	}
	id @0 :Data;
	type @1 :RequestType;
	ip @2 :Text;
	port @3 :UInt32;
	forward @4 :Bool;
}

