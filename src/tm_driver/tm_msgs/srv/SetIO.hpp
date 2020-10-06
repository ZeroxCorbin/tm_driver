namespace tm_msgs {
	namespace srv {
		namespace SetIO {
			struct Request {
				const static __int8 MODULE_CONTROLBOX = 0;
				const static __int8 MODULE_ENDEFFECTOR = 1;

				const static __int8 TYPE_DIGITAL_IN = 0;
				const static __int8 TYPE_DIGITAL_OUT = 1;
				const static __int8 TYPE_INSTANT_DO = 2;
				const static __int8 TYPE_ANALOG_IN = 3;
				const static __int8 TYPE_ANALOG_OUT = 4;
				const static __int8 TYPE_INSTANT_AO = 5;

				const static __int8 STATE_OFF = 0;
				const static __int8 STATE_ON = 1;

				// request fields
				__int8 module;
				__int8 type;
				__int8 pin;
				float state;
			};
			struct Response {
				//ok = set function status
				bool ok;
			};
		};
	};
};