// Ask: request 'Read'
// id        : The discribtion you want to name. 
// item      : More details please refer to the TM_Robot_Expression.pdf Chapter 10.2
// wait_time : set wait time.
// More details please refer to the TM_Robot_Expression.pdf Chapter 10.2

namespace tm_msgs {
	namespace srv {
		namespace AskItem {
			struct Request {
				std::string id;
				std::string item;

				float wait_time;
			};
			struct Response {
				// ok        : read status 
				bool ok;

				// id        : The discribtion you want to name.
				// value     : The value of asked item.
				std::string id;
				std::string value;
			};
		};
	};
};