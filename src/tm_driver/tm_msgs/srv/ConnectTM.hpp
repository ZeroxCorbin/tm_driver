//float64 DEFAULT_TIMEOUT = 1.0
//float64 DEFAULT_TIMEVAL = 3.0
//server :   TMSVR, TMSCT
//connect/reconnect : In Connection phase. 
//connect    == true. Keeping connect function
//reconnect  == true. If connect funcition fail, It will reconnect.

namespace tm_msgs {
	namespace srv {
		namespace ConnectTM {
			struct Request {
				__int8 TMSVR = 0;
				__int8 TMSCT = 1;

				__int8 server;

				bool connect;

				bool reconnect;

				float timeout;

				float timeval;
			};
			struct Response {
				//ok :   connect status
				bool ok;

			};
		};

	};


};

