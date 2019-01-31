#include <dht/localserviceaddresslookup.hpp>

#include <dht/context.hpp>
#include <dht/messages/gotintro.hpp>
#include <router/router.hpp>
#include <util/logger.hpp>

namespace llarp
{
  namespace dht
  {
    LocalServiceAddressLookup::LocalServiceAddressLookup(
        const PathID_t &pathid, uint64_t txid, const service::Address &addr,
        AbstractContext *ctx, __attribute__((unused)) const Key_t &askpeer)
        : ServiceAddressLookup(TXOwner{ctx->OurKey(), txid}, addr, ctx, 5,
                               nullptr)
        , localPath(pathid)
    {
    }

    void
    LocalServiceAddressLookup::SendReply()
    {
      auto path = parent->GetRouter()->paths.GetByUpstream(
          parent->OurKey().as_array(), localPath);
      if(!path)
      {
        llarp::LogWarn(
            "did not send reply for relayed dht request, no such local path "
            "for pathid=",
            localPath);
        return;
      }
      routing::DHTMessage msg;
      msg.M.emplace_back(new GotIntroMessage(valuesFound, whoasked.txid));
      if(!path->SendRoutingMessage(&msg, parent->GetRouter()))
      {
        llarp::LogWarn(
            "failed to send routing message when informing result of dht "
            "request, pathid=",
            localPath);
      }
    }
  }  // namespace dht
}  // namespace llarp