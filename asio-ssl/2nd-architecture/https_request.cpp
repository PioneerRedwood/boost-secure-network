// request property
// request_type: GET, POST
// URL: example.com
// Path: /path/
// 

/**
 * can is the socket reusable?
 * @brief The process for requesting https
 * 
 * resolve
 * connect
 * send request -- it can be called simultanuesly
 * receive below as a proper sequence
 *  status_code
 *  header
 *  body
 */