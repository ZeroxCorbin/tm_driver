// 'Write'
// id        : The discribtion you want to name. 
// item/value: More details please refer to the TM_Robot_Expression.pdf Chapter 10.3

namespace tm_msgs {
	namespace srv {
		namespace WriteItem {
			struct Request {
				std::string id;
				std::string item;
				std::string value;
			};
			struct Response {
				//ok =   write status
				bool ok;
			};
		};
	};
};