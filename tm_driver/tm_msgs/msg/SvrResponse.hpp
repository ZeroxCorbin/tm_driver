namespace tm_msgs {

	namespace msg {
		struct SvrResponse {
			std::string id;
			__int8 mode;
			std::string content;
			__int8 error_code;
		};
	};
};
