// More details please refer to the tm_eos_service.cpp/set_event function.

namespace tm_msgs {
	namespace srv {
		namespace SetEvent{
			struct Request {
				const static __int8 TAG = 1;
				const static __int8 WAIT_TAG = 2;
				const static __int8 STOP = 11;
				const static __int8 PAUSE = 12;
				const static __int8 RESUME = 13;

				const static __int8 EXIT = -1;

				__int8 func;

				__int8 arg0;
				__int8 arg1;
			};
			struct Response {
				// ok == complete status
				bool ok;
				__int8 rv;
			};
		};
	};
};
