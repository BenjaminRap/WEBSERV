#include "ARequestType.hpp"

std::map<int, std::string>	getStatuses(void)
{
	std::map<int, std::string>	_statuses;

    _statuses.insert(std::make_pair(100, "Continue"));
    _statuses.insert(std::make_pair(101, "Switching Protocols"));
    _statuses.insert(std::make_pair(102, "Processing"));
    _statuses.insert(std::make_pair(103, "Early Hints"));

    // 2xx - Succès
    _statuses.insert(std::make_pair(200, "OK"));
    _statuses.insert(std::make_pair(201, "Created"));
    _statuses.insert(std::make_pair(202, "Accepted"));
    _statuses.insert(std::make_pair(203, "Non-Authoritative Information"));
    _statuses.insert(std::make_pair(204, "No Content"));
    _statuses.insert(std::make_pair(205, "Reset Content"));
    _statuses.insert(std::make_pair(206, "Partial Content"));
    _statuses.insert(std::make_pair(207, "Multi-Status"));
    _statuses.insert(std::make_pair(208, "Already Reported"));
    _statuses.insert(std::make_pair(226, "IM Used"));

    // 3xx - Redirection
    _statuses.insert(std::make_pair(300, "Multiple Choices"));
    _statuses.insert(std::make_pair(301, "Moved Permanently"));
    _statuses.insert(std::make_pair(302, "Found"));
    _statuses.insert(std::make_pair(303, "See Other"));
    _statuses.insert(std::make_pair(304, "Not Modified"));
    _statuses.insert(std::make_pair(307, "Temporary Redirect"));
    _statuses.insert(std::make_pair(308, "Permanent Redirect"));

    // 4xx - Erreurs client
    _statuses.insert(std::make_pair(400, "Bad Request"));
    _statuses.insert(std::make_pair(401, "Unauthorized"));
    _statuses.insert(std::make_pair(402, "Payment Required"));
    _statuses.insert(std::make_pair(403, "Forbidden"));
    _statuses.insert(std::make_pair(404, "Not Found"));
    _statuses.insert(std::make_pair(405, "Method Not Allowed"));
    _statuses.insert(std::make_pair(406, "Not Acceptable"));
    _statuses.insert(std::make_pair(407, "Proxy Authentication Required"));
    _statuses.insert(std::make_pair(408, "Request Timeout"));
    _statuses.insert(std::make_pair(409, "Conflict"));
    _statuses.insert(std::make_pair(410, "Gone"));
    _statuses.insert(std::make_pair(411, "Length Required"));
    _statuses.insert(std::make_pair(412, "Precondition Failed"));
    _statuses.insert(std::make_pair(413, "Payload Too Large"));
    _statuses.insert(std::make_pair(414, "URI Too Long"));
    _statuses.insert(std::make_pair(415, "Unsupported Media Type"));
    _statuses.insert(std::make_pair(416, "Range Not Satisfiable"));
    _statuses.insert(std::make_pair(417, "Expectation Failed"));
    _statuses.insert(std::make_pair(418, "I'm a teapot"));
    _statuses.insert(std::make_pair(421, "Misdirected Request"));
    _statuses.insert(std::make_pair(422, "Unprocessable Entity"));
    _statuses.insert(std::make_pair(423, "Locked"));
    _statuses.insert(std::make_pair(424, "Failed Dependency"));
    _statuses.insert(std::make_pair(425, "Too Early"));
    _statuses.insert(std::make_pair(426, "Upgrade Required"));
    _statuses.insert(std::make_pair(428, "Precondition Required"));
    _statuses.insert(std::make_pair(429, "Too Many Requests"));
    _statuses.insert(std::make_pair(431, "Request Header Fields Too Large"));
    _statuses.insert(std::make_pair(451, "Unavailable For Legal Reasons"));

    // 5xx - Erreurs serveur
    _statuses.insert(std::make_pair(500, "Internal Server Error"));
    _statuses.insert(std::make_pair(501, "Not Implemented"));
    _statuses.insert(std::make_pair(502, "Bad Gateway"));
    _statuses.insert(std::make_pair(503, "Service Unavailable"));
    _statuses.insert(std::make_pair(504, "Gateway Timeout"));
    _statuses.insert(std::make_pair(505, "HTTP Version Not Supported"));
    _statuses.insert(std::make_pair(506, "Variant Also Negotiates"));
    _statuses.insert(std::make_pair(507, "Insufficient Storage"));
    _statuses.insert(std::make_pair(508, "Loop Detected"));
    _statuses.insert(std::make_pair(510, "Not Extended"));
    _statuses.insert(std::make_pair(511, "Network Authentication Required"));
	return (_statuses);
}

const std::map<int, std::string>	ARequestType::_statuses = getStatuses();
