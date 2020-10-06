// Ask: request 'Read' sta
// More details please refer to the TM_Robot_Expression.pdf Chapter 8.5
// wait_time : set wait time.
namespace tm_msgs {
	namespace srv {
		namespace AskSta {
			struct Request {
				std::string subcmd;
				std::string subdata;

				float wait_time;
			};
			struct Response {
				std::string subcmd;
				std::string subdata;

				// ok        : read status 
				bool ok;
			};
		};
	};
};