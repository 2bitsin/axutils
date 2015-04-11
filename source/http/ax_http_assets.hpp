#ifndef __AX_HTTP_ASSETS_HPP__
#define __AX_HTTP_ASSETS_HPP__

#include <string>

namespace ax{
    namespace http{
        struct assets {
            static std::string get_head () {
                return R"(
                    <link rel="stylesheet" type="text/css" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css" />
                    <script type="application/javascript" src="https://code.jquery.com/jquery-2.1.3.min.js"></script>
                    <script type="application/javascript" src="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>
                )";
            }

            template <typename _Request, typename _Response>
            static bool send_404 (_Request &&request_, _Response &&response_) {
                response_.header ("HTTP/1.1 404 Page not found");
                response_.header ("Content-Type", "text/html");            
                response_.send (std::string (
                    "<html><head>"+get_head ()+"<title>404 - Page not found</title></head>"
                    "<body><div class='container' style='padding-top:20pt'>"
                    "<div class='jumbotron'><h1>404 - Page not found</h1>"
                    "<p>The URL you are looking for is not available.</p>"
                    "</div></div></body></html>"));
                return true;
            };

            template <typename _Request, typename _Response, typename _What>
            static bool send_500 (_Request &&request_, _Response &&response_, _What &&what_) {
                response_.header ("HTTP/1.1 500 Internal server error");
                response_.header ("Content-Type", "text/html");            
                response_.send (std::string (
                    "<html><head>"+get_head ()+"<title>500 - Internal server error</title></head>"
                    "<body><div class='container' style='padding-top:20pt'><div class='jumbotron'>"
                    "<h1>500 - Internal server errror</h1><p>"+std::string (what_.what ())+
                    ".</p></div></div></body></html>"));
                return true;
            };

            template <typename _Request, typename _Response, typename _Index>
            static bool generate_directory_listing (
                _Request &&req_, 
                _Response &&resp_,
                _Index begin_, 
                _Index end_) 
            {
                resp_.header ("Content-Type", "text/html");
                std::string index_ = 
                    "<html><head><title>"+req_.path ()+
                    "</title>"+get_head ()+"</head><body>"
                    "<div class='container'><h1>"+req_.path ()+
                    "</h1><table class='table table-striped'>";
                resp_.header ("Content-Type", "text/html");
                for (auto i = begin_; i != end_; ++i) {                                    
                    index_.append ("<tr><td style='width:30pt'>"
                        "<span class='glyphicon glyphicon-"+
                        std::string (i->directory () ? (i->up_directory () ? 
                            "folder-close" : "folder-open") : "file")+
                        "'></span></td><td><a href=\""+i->href ()+"\">"+
                        i->title ()+"</a></td></tr>");
                }
                index_.append ("</table></div></body></html>");
                resp_.send (index_);
                return true;
            }
        };
    }
}

#endif



