#include "tm_driver/tm_driver.h"
#include "tm_driver/tm_print.h"

#include <SctResponse.hpp>
#include <StaResponse.hpp>
#include <ConnectTM.hpp>
#include <SendScript.hpp>
#include <SetEvent.hpp>
#include <SetPositions.hpp>
#include <SetIO.hpp>
#include <AskSta.hpp>
#include <iostream>

TmDriver* iface_;

std::mutex sta_mtx_;
std::condition_variable sta_cond_;

std::condition_variable psrv_cv_cond_;
std::condition_variable pscv_cv_cond_;

bool sta_updated_;
int sct_reconnect_timeval_ms_;
int sct_reconnect_timeout_ms_;


struct SctMsg {
    tm_msgs::msg::SctResponse sct_msg;
    tm_msgs::msg::StaResponse sta_msg;
} sm_;

int main(int argc, char* argv[]) {

    std::string host;
    if (argc > 1) {
        host = argv[1];
    }
    iface_ = new TmDriver(host, &psrv_cv_cond_, &pscv_cv_cond_);

    iface_->start();
    
    while (true) {
        std::string test;
        std::getline(std::cin, test);

        if (test == "s")
            iface_->state.print();
        else
            iface_->set_stop();
    };

	return 0;
}

bool connect_tmsct(
    const std::shared_ptr<tm_msgs::srv::ConnectTM::Request> req,
    std::shared_ptr<tm_msgs::srv::ConnectTM::Response> res)
{
    bool rb = true;
    int t_o = (int)(1000.0 * req->timeout);
    int t_v = (int)(1000.0 * req->timeval);
    if (req->connect) {
        print_info("(TM_SCT) (re)connect(%d) TM_SCT", t_o);
        iface_->sct.halt();
        rb = iface_->sct.start(t_o);
    }
    if (req->reconnect) {
        sct_reconnect_timeout_ms_ = t_o;
        sct_reconnect_timeval_ms_ = t_v;
        print_info("(TM_SCT) set SCT reconnect timeout %dms, timeval %dms", t_o, t_v);
    }
    else {
        // no reconnect
        sct_reconnect_timeval_ms_ = -1;
        print_info("(TM_SCT) set SCT NOT reconnect");
    }
    res->ok = rb;
    return rb;
}

bool send_script(
    const std::shared_ptr<tm_msgs::srv::SendScript::Request> req,
    std::shared_ptr<tm_msgs::srv::SendScript::Response> res)
{
    bool rb = (iface_->sct.send_script_str(req->id, req->script) == iface_->RC_OK);
    res->ok = rb;
    return rb;
}
bool set_event(
    const std::shared_ptr<tm_msgs::srv::SetEvent::Request> req,
    std::shared_ptr<tm_msgs::srv::SetEvent::Response> res)
{
    bool rb = false;
    std::string content;
    switch (req->func) {
    case tm_msgs::srv::SetEvent::Request::EXIT:
        rb = iface_->script_exit();
        break;
    case tm_msgs::srv::SetEvent::Request::TAG:
        rb = iface_->set_tag((int)(req->arg0), (int)(req->arg1));
        break;
    case tm_msgs::srv::SetEvent::Request::WAIT_TAG:
        rb = iface_->set_wait_tag((int)(req->arg0), (int)(req->arg1));
        break;
    case tm_msgs::srv::SetEvent::Request::STOP:
        rb = iface_->set_stop();
        break;
    case tm_msgs::srv::SetEvent::Request::PAUSE:
        rb = iface_->set_pause();
        break;
    case tm_msgs::srv::SetEvent::Request::RESUME:
        rb = iface_->set_resume();
        break;
    }
    res->ok = rb;
    return rb;
}
bool set_io(
    const std::shared_ptr<tm_msgs::srv::SetIO::Request> req,
    std::shared_ptr<tm_msgs::srv::SetIO::Response> res)
{
    bool rb = iface_->set_io(TmIOModule(req->module), TmIOType(req->type), int(req->pin), req->state);
    res->ok = rb;
    return rb;
}
bool set_positions(
    const std::shared_ptr<tm_msgs::srv::SetPositions::Request> req,
    std::shared_ptr<tm_msgs::srv::SetPositions::Response> res)
{
    bool rb = false;
    switch (req->motion_type) {
    case tm_msgs::srv::SetPositions::Request::PTP_J:
        rb = iface_->set_joint_pos_PTP(req->positions, req->velocity, req->acc_time, req->blend_percentage, req->fine_goal);
        break;
    case tm_msgs::srv::SetPositions::Request::PTP_T:
        rb = iface_->set_tool_pose_PTP(req->positions, req->velocity, req->acc_time, req->blend_percentage, req->fine_goal);
        break;
    case tm_msgs::srv::SetPositions::Request::LINE_T:
        rb = iface_->set_tool_pose_Line(req->positions, req->velocity, req->acc_time, req->blend_percentage, req->fine_goal);
        break;
    }
    res->ok = rb;
    return rb;
}

bool ask_sta(
    const std::shared_ptr<tm_msgs::srv::AskSta::Request> req,
    std::shared_ptr<tm_msgs::srv::AskSta::Response> res)
{
    SctMsg& sm = sm_;
    bool rb = false;

    sta_updated_ = false;

    rb = (iface_->sct.send_script_str(req->subcmd, req->subdata) == iface_->RC_OK);

    if (req->wait_time > 0.0) {
        std::mutex lock;
        std::unique_lock<std::mutex> locker(lock);
        if (!sta_updated_) {
            sta_cond_.wait_for(locker, std::chrono::duration<double>(req->wait_time));
        }
        if (sta_updated_) {
            sta_mtx_.lock();
            res->subcmd = sm.sta_msg.subcmd;
            res->subdata = sm.sta_msg.subdata;
            sta_mtx_.unlock();
            sta_updated_ = false;
        }
        else rb = false;
    }
    res->ok = rb;
    return rb;
}
//void sct_msg()
//{
//    SctMsg& sm = sm_;
//    TmSctData& data = iface_->sct.sct_data;
//
//    sm.sct_msg.id = data.script_id();
//    sm.sct_msg.script = std::string{ data.script(), data.script_len() };
//
//    if (data.has_error()) {
//        print_info("(TM_SCT): err: (%s): %s", sm.sct_msg.id.c_str(), sm.sct_msg.script.c_str());
//    }
//    else {
//        print_info("(TM_SCT): res: (%s): %s", sm.sct_msg.id.c_str(), sm.sct_msg.script.c_str());
//    }
//
//    //sm.sct_msg.header.stamp = rclcpp::Node::now();
//    //sm.sct_pub->publish(sm.sct_msg);
//}
//void sta_msg()
//{
//    SctMsg& sm = sm_;
//    TmStaData& data = iface_->sct.sta_data;
//
//    sta_mtx_.lock();
//    sm.sta_msg.subcmd = data.subcmd_str();
//    sm.sta_msg.subdata = std::string{ data.subdata(), data.subdata_len() };
//    sta_mtx_.unlock();
//
//    sta_updated_ = true;
//    sta_cond_.notify_all();
//
//    print_info("(TM_STA): res: (%s): %s", sm.sta_msg.subcmd.c_str(), sm.sta_msg.subdata.c_str());
//
//    //sm.sta_msg.header.stamp = rclcpp::Node::now();
//    //sm.sta_pub->publish(sm.sta_msg);
//}
//bool sct_func()
//{
//    TmSctCommunication& sct = iface_->sct;
//    int n;
//    auto rc = sct.recv_spin_once(1000, &n);
//    if (rc == TmCommRC::ERR ||
//        rc == TmCommRC::NOTREADY ||
//        rc == TmCommRC::NOTCONNECT) {
//        return false;
//    }
//    else if (rc != TmCommRC::OK) {
//        return true;
//    }
//    std::vector<TmPacket>& pack_vec = sct.packet_list();
//
//    for (auto& pack : pack_vec) {
//        switch (pack.type) {
//        case TmPacket::Header::CPERR:
//            print_info("(TM_SCT): CPERR");
//            sct.err_data.set_CPError(pack.data.data(), pack.data.size());
//            print_error(sct.err_data.error_code_str().c_str());
//
//            // cpe response
//
//            break;
//
//        case TmPacket::Header::TMSCT:
//
//            sct.err_data.error_code(TmCPError::Code::Ok);
//
//            //TODO ? lock and copy for service response
//            TmSctData::build_TmSctData(sct.sct_data, pack.data.data(), pack.data.size(), TmSctData::SrcType::Shallow);
//
//            sct_msg();
//            break;
//
//        case TmPacket::Header::TMSTA:
//
//            sct.err_data.error_code(TmCPError::Code::Ok);
//
//            TmStaData::build_TmStaData(sct.sta_data, pack.data.data(), pack.data.size(), TmStaData::SrcType::Shallow);
//
//            sta_msg();
//            break;
//
//        default:
//            print_info("(TM_SCT): invalid header");
//            break;
//        }
//    }
//    return true;
//}
//void sct_responsor()
//{
//    TmSctCommunication& sct = iface_->sct;
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(50));
//
//    print_info("(TM_SCT): sct_response thread begin");
//
//    while (true) {
//        //bool reconnect = false;
//        if (!sct.recv_init()) {
//            print_info("(TM_SCT) is not connected");
//        }
//        while (sct.is_connected()) {
//            if (!sct_func()) break;
//        }
//        sct.Close();
//
//        // reconnect == true
//        //if (!rclcpp::ok()) break;
//        if (sct_reconnect_timeval_ms_ <= 0) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//        print_info("(TM_SCT) reconnect in ");
//        int cnt = 0;
//        while (cnt < sct_reconnect_timeval_ms_) {
//            if (cnt % 1000 == 0) {
//                print_info("%.1f sec...", 0.001 * (sct_reconnect_timeval_ms_ - cnt));
//            }
//            std::this_thread::sleep_for(std::chrono::milliseconds(1));
//            ++cnt;
//        }
//        if (sct_reconnect_timeval_ms_ >= 0) {
//            print_info("0 sec\n(TM_SCT) connect(%d)...", sct_reconnect_timeout_ms_);
//            sct.Connect(sct_reconnect_timeout_ms_);
//        }
//    }
//    sct.Close();
//    printf("(TM_SCT) sct_response thread end\n");
//}

