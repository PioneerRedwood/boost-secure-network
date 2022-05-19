/*
 * Modified from
 * http://stackoverflow.com/a/11044337/2206385
 */
#include <string>
#include <algorithm> // find

// protocol host port path query
struct Uri {
    public:
        std::string query, path, protocol, host, port;

    static Uri parse(const std::string &uri) {
        Uri result;

        typedef std::string::const_iterator iterator_t;

        if (uri.length() == 0)
            return result;

        iterator_t uriEnd = uri.end();

        // Get query start
        iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

        // protocol
        iterator_t protocolStart = uri.begin();
        // "://"
        iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');

        if (protocolEnd != uriEnd) {
            std::string prot = &*(protocolEnd);
            if ((prot.length() > 3) && (prot.substr(0, 3) == "://")) {
                result.protocol = std::string(protocolStart, protocolEnd);
                protocolEnd += 3;   // ://
            } else {
                // No protocol
                protocolEnd = uri.begin();
            }
        } else {
            // No protocol
            protocolEnd = uri.begin();
        }

        // host
        iterator_t hostStart = protocolEnd;
        // Get pathStart
        iterator_t pathStart = std::find(hostStart, uriEnd, '/');
        // Check for port
        iterator_t hostEnd = std::find(protocolEnd,
            (pathStart != uriEnd) ? pathStart : queryStart, ':');

        if (!result.protocol.empty()) {
            result.host = std::string(hostStart, hostEnd);
        }
        // port
        if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':')) {
            // We have a port
            hostEnd++;
            iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
            result.port = std::string(hostEnd, portEnd);
        }
        // path
        if (pathStart != uriEnd) {
            result.path = std::string(pathStart, queryStart);
        }
        // Query
        if (queryStart != uriEnd) {
            result.query = std::string(queryStart, uri.end());
        }
        return result;

    }
};