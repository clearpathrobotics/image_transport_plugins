#include <image_transport/simple_publisher_plugin.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <cv_bridge/CvBridge.h>
#include <theora_image_transport/Packet.h>

#include <theora/codec.h>
#include <theora/theoraenc.h>
#include <theora/theoradec.h>

namespace theora_image_transport {

class TheoraPublisher : public image_transport::SimplePublisherPlugin<theora_image_transport::Packet>
{
public:
  TheoraPublisher();

  ~TheoraPublisher();
  
  // Return the system unique string representing the theora transport type
  virtual std::string getTransportName() const { return "theora"; }

protected:
  // Overridden to tweak queue_size to make sure headers don't get lost
  virtual void advertiseImpl(ros::NodeHandle &nh, const std::string &base_topic, uint32_t queue_size,
                             const image_transport::SubscriberStatusCallback  &user_connect_cb,
                             const image_transport::SubscriberStatusCallback  &user_disconnect_cb,
                             const ros::VoidPtr &tracked_object, bool latch);
  
  // Callback to send header packets to new clients
  virtual void connectCallback(const ros::SingleSubscriberPublisher& pub);

  // Main publish function
  virtual void publish(const sensor_msgs::Image& message,
                       const PublishFn& publish_fn) const;

private:
  // Utility functions
  void sendHeader(const ros::SingleSubscriberPublisher& pub) const;
  void ensure_encoding_context(const sensor_msgs::Image& image, const PublishFn& publish_fn) const;
  void oggPacketToMsg(const roslib::Header& header, const ogg_packet &oggpacket,
                      theora_image_transport::Packet &msg) const;

  // Some data is preserved across calls to publish(), but from the user's perspective publish() is
  // "logically const"
  mutable sensor_msgs::CvBridge img_bridge_;
  mutable th_info encoder_setup_;
  mutable boost::shared_ptr<th_enc_ctx> encoding_context_;
  mutable std::vector<theora_image_transport::Packet> stream_header_;
};

} //namespace compressed_image_transport
