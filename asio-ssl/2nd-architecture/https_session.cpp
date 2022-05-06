#include "https_core.h"

// how can manage this instance ? 
class session : public std::enable_shared_from_this<session>
{
private:

public:
// consturct, destructor
  session() {}
  ~session() {}

private:
  // -- reference instance --
  // io, ssl context
  
  // -- normal instance --
  // ssl tcp socket
  // stream for request, reseponse 

  // -- shared instance -- 
  // http request deque shared ptr
  // http response deque shared ptr
  
  // -- singleton instance --
  // cache manager
}