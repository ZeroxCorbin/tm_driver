//motion_type :  PTP_J , PTP_T , LINE_J , LINE_T ,
//               CIRC_J ,CIRC_T , PLINE_J ,PLINE_T
// More details please refer to the TM_Robot_Expression.pdf Chapter 9.6-9.9
namespace tm_msgs {
	namespace srv {
		namespace SetPositions {
			struct Request {
				const static __int8 PTP_J = 1;
				const static __int8 PTP_T = 2;
				//int8 LINE_J = 3;
				const static __int8 LINE_T = 4;
				//int8 CIRC_J = 5;
				const static __int8 CIRC_T = 6;
				//int8 PLINE_J = 7;
				const static __int8 PLINE_T = 8;

				__int8 motion_type;
				const std::vector<double> positions;
				float velocity;
				float acc_time;
				int blend_percentage;
				bool fine_goal;
			};
			struct Response {
				// ok :  set motion status 
				bool ok;
			};
		};
	};
};
