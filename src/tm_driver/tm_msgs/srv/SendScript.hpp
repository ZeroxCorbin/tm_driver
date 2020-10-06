
namespace tm_msgs {
	namespace srv {
		namespace SendScript {
			struct Request {
				std::string id;
				std::string script;
			};
			struct Response {
				bool ok;
			};
		};
	};
};