//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <thread>

using namespace beast;

//[http_snippet_1

#include <beast/http.hpp>
using namespace beast::http;

//]

namespace doc_http_snippets {

//[http_snippet_17
// This function returns the buffer containing the next chunk body
boost::asio::const_buffers_1 get_next_chunk_body();
//]

boost::asio::const_buffers_1 get_next_chunk_body()
{
    return {nullptr, 0};
}

void fxx() {

    boost::asio::io_service ios;
    boost::asio::io_service::work work{ios};
    std::thread t{[&](){ ios.run(); }};
    boost::asio::ip::tcp::socket sock{ios};

{
//[http_snippet_2

    request<empty_body> req;
    req.version = 11;   // HTTP/1.1
    req.method(verb::get);
    req.target("/index.htm");
    req.set(field::accept, "text/html");
    req.set(field::user_agent, "Beast");

//]
}

{
//[http_snippet_3

    response<string_body> res;
    res.version = 11;   // HTTP/1.1
    res.result(status::ok);
    res.set(field::server, "Beast");
    res.body = "Hello, world!";
    res.prepare_payload();

//]
}

{
//[http_snippet_4

    flat_buffer buffer;         // (The parser is optimized for flat buffers)
    request<string_body> req;
    read(sock, buffer, req);

//]
}

{
//[http_snippet_5

    flat_buffer buffer;
    response<string_body> res;
    async_read(sock, buffer, res,
        [&](error_code ec)
        {
            std::cerr << ec.message() << std::endl;
        });

//]
}

{
//[http_snippet_6

    // This buffer's max size is too small for much of anything
    flat_buffer buffer{10};

    // Try to read a request
    error_code ec;
    request<string_body> req;
    read(sock, buffer, req, ec);
    if(ec == error::buffer_overflow)
        std::cerr << "Buffer limit exceeded!" << std::endl;

//]
}

{
//[http_snippet_7

    response<string_body> res;
    res.version = 11;
    res.result(status::ok);
    res.set(field::server, "Beast");
    res.body = "Hello, world!";

    error_code ec;
    write(sock, res, ec);
    if(ec == error::end_of_stream)
        sock.close();
//]

//[http_snippet_8
    async_write(sock, res,
        [&](error_code)
        {
            if(ec)
                std::cerr << ec.message() << std::endl;
        });
//]
}

{
//[http_snippet_10

    response<string_body> res;

    response_serializer<string_body> sr{res};

//]
}

{
//[http_snippet_18
    // Prepare an HTTP/1.1 response with a chunked body
    response<empty_body> res{status::ok, 11};
    res.set(field::server, "Beast");

    // Set Transfer-Encoding to "chunked".
    // If a Content-Length was present, it is removed.
    res.chunked(true);

    // Set up the serializer
    response_serializer<empty_body> sr{res};

    // Write the header first
    write_header(sock, sr);

    // Now manually emit three chunks:
    boost::asio::write(sock, make_chunk(get_next_chunk_body()));
    boost::asio::write(sock, make_chunk(get_next_chunk_body()));
    boost::asio::write(sock, make_chunk(get_next_chunk_body()));

    // We are responsible for sending the last chunk:
    boost::asio::write(sock, make_chunk_last());
//]
}

{
//[http_snippet_19
    // Prepare a set of chunk extension to emit with the body
    chunk_extensions ext;
    ext.insert("mp3");
    ext.insert("title", "Beale Street Blues");
    ext.insert("artist", "W.C. Handy");

    // Write the next chunk with the chunk extensions
    // The implementation will make a copy of the extensions object,
    // so the caller does not need to manage lifetime issues.
    boost::asio::async_write(sock, make_chunk(get_next_chunk_body(), ext),
        [](error_code ec, std::size_t)
        {
            if(ec)
                std::cout << "Error: " << ec.message() << std::endl;
        });

    // Write the next chunk with the chunk extensions
    // The implementation will make a copy of the extensions object, storing the copy
    // using the custom allocator, so the caller does not need to manage lifetime issues.
    boost::asio::async_write(sock,
        make_chunk(get_next_chunk_body(), ext, std::allocator<char>{}),
        [](error_code ec, std::size_t)
        {
            if(ec)
                std::cout << "Error: " << ec.message() << std::endl;
        });

    // Write the next chunk with the chunk extensions
    // The implementation allocates memory using the default allocator and takes ownership
    // of the extensions object, so the caller does not need to manage lifetime issues.
    // Note: ext is moved
    boost::asio::async_write(sock, make_chunk(get_next_chunk_body(), std::move(ext)),
        [](error_code ec, std::size_t)
        {
            if(ec)
                std::cout << "Error: " << ec.message() << std::endl;
        });
//]
}

{
//[http_snippet_20
    // Manually specify the chunk extensions.
    // Some of the strings contain spaces and a period and must be quoted
    boost::asio::write(sock, make_chunk(get_next_chunk_body(),
        ";mp3"
        ";title=\"Danny Boy\""
        ";artist=\"Fred E. Weatherly\""
        ));
//]
}

{
//[http_snippet_21
    // Prepare a chunked HTTP/1.1 response with some trailer fields
    response<empty_body> res{status::ok, 11};
    res.set(field::server, "Beast");

    // Inform the client of the trailer fields we will send
    res.set(field::trailer, "Content-MD5, Expires");

    res.chunked(true);

    // Serialize the header and two chunks
    response_serializer<empty_body> sr{res};
    write_header(sock, sr);
    boost::asio::write(sock, make_chunk(get_next_chunk_body()));
    boost::asio::write(sock, make_chunk(get_next_chunk_body()));

    // Prepare the trailer
    fields trailer;
    trailer.set(field::content_md5, "f4a5c16584f03d90");
    trailer.set(field::expires, "never");

    // Emit the trailer in the last chunk.
    // The implementation will use the default allocator to create the storage for holding
    // the serialized fields.
    boost::asio::write(sock, make_chunk_last(trailer));
//]
}

{
//[http_snippet_22
    // Use a custom allocator for serializing the last chunk
    fields trailer;
    trailer.set(field::approved, "yes");
    boost::asio::write(sock, make_chunk_last(trailer, std::allocator<char>{}));
//]
}

{
//[http_snippet_23
    // Manually emit a trailer.
    // We are responsible for ensuring that the trailer format adheres to the specification.
    string_view ext =
        "Content-MD5: f4a5c16584f03d90\r\n"
        "Expires: never\r\n"
        "\r\n";
    boost::asio::write(sock, make_chunk_last(boost::asio::const_buffers_1{ext.data(), ext.size()}));
//]
}

{
//[http_snippet_24
    // Prepare a chunked HTTP/1.1 response and send the header
    response<empty_body> res{status::ok, 11};
    res.set(field::server, "Beast");
    res.chunked(true);
    response_serializer<empty_body> sr{res};
    write_header(sock, sr);

    // Obtain three body buffers up front
    auto const cb1 = get_next_chunk_body();
    auto const cb2 = get_next_chunk_body();
    auto const cb3 = get_next_chunk_body();

    // Manually emit a chunk by first writing the chunk-size header with the correct size
    boost::asio::write(sock, chunk_header{
        boost::asio::buffer_size(cb1) +
        boost::asio::buffer_size(cb2) +
        boost::asio::buffer_size(cb3)});

    // And then output the chunk body in three pieces ("chunk the chunk")
    boost::asio::write(sock, cb1);
    boost::asio::write(sock, cb2);
    boost::asio::write(sock, cb3);

    // When we go this deep, we are also responsible for the terminating CRLF
    boost::asio::write(sock, chunk_crlf{});
//]
}

} // fxx()



//[http_snippet_12

/** Send a message to a stream synchronously.

    @param stream The stream to write to. This type must support
    the @b SyncWriteStream concept.

    @param m The message to send. The Body type must support
    the @b BodyReader concept.
*/
template<
    class SyncWriteStream,
    bool isRequest, class Body, class Fields>
void
send(
    SyncWriteStream& stream,
    message<isRequest, Body, Fields> const& m)
{
    // Check the template types
    static_assert(is_sync_write_stream<SyncWriteStream>::value,
        "SyncWriteStream requirements not met");
    static_assert(is_body_reader<Body>::value,
        "BodyReader requirements not met");

    // Create the instance of serializer for the message
    serializer<isRequest, Body, Fields> sr{m};

    // Loop until the serializer is finished
    do
    {
        // This call guarantees it will make some
        // forward progress, or otherwise return an error.
        write_some(stream, sr);
    }
    while(! sr.is_done());
}

//]

//[http_snippet_13

template<class SyncReadStream>
void
print_response(SyncReadStream& stream)
{
    static_assert(is_sync_read_stream<SyncReadStream>::value,
        "SyncReadStream requirements not met");

    // Declare a parser for an HTTP response
    response_parser<string_body> parser;

    // Read the entire message
    read(stream, parser);

    // Now print the message
    std::cout << parser.get() << std::endl;
}

//]

#ifdef BOOST_MSVC
//[http_snippet_14

template<bool isRequest, class Body, class Fields>
void
print_cxx14(message<isRequest, Body, Fields> const& m)
{
    error_code ec;
    serializer<isRequest, Body, Fields> sr{m};
    do
    {
        sr.next(ec,
            [&sr](error_code& ec, auto const& buffer)
            {
                ec.assign(0, ec.category());
                std::cout << buffers(buffer);
                sr.consume(boost::asio::buffer_size(buffer));
            });
    }
    while(! ec && ! sr.is_done());
    if(! ec)
        std::cout << std::endl;
    else
        std::cerr << ec.message() << std::endl;
}

//]
#endif

//[http_snippet_15

template<class Serializer>
struct lambda
{
    Serializer& sr;

    lambda(Serializer& sr_) : sr(sr_) {}

    template<class ConstBufferSequence>
    void operator()(error_code& ec, ConstBufferSequence const& buffer) const
    {
        ec.assign(0, ec.category());
        std::cout << buffers(buffer);
        sr.consume(boost::asio::buffer_size(buffer));
    }
};

template<bool isRequest, class Body, class Fields>
void
print(message<isRequest, Body, Fields> const& m)
{
    error_code ec;
    serializer<isRequest, Body, Fields> sr{m};
    do
    {
        sr.next(ec, lambda<decltype(sr)>{sr});
    }
    while(! ec && ! sr.is_done());
    if(! ec)
        std::cout << std::endl;
    else
        std::cerr << ec.message() << std::endl;
}

//]

#if BOOST_MSVC
//[http_snippet_16

template<bool isRequest, class Body, class Fields>
void
split_print_cxx14(message<isRequest, Body, Fields> const& m)
{
    error_code ec;
    serializer<isRequest, Body, Fields> sr{m};
    sr.split(true);
    std::cout << "Header:" << std::endl;
    do
    {
        sr.next(ec,
            [&sr](error_code& ec, auto const& buffer)
            {
                ec.assign(0, ec.category());
                std::cout << buffers(buffer);
                sr.consume(boost::asio::buffer_size(buffer));
            });
    }
    while(! sr.is_header_done());
    if(! ec && ! sr.is_done())
    {
        std::cout << "Body:" << std::endl;
        do
        {
            sr.next(ec,
                [&sr](error_code& ec, auto const& buffer)
                {
                    ec.assign(0, ec.category());
                    std::cout << buffers(buffer);
                    sr.consume(boost::asio::buffer_size(buffer));
                });
        }
        while(! ec && ! sr.is_done());
    }
    if(ec)
        std::cerr << ec.message() << std::endl;
}

//]
#endif

// Highest snippet: 

} // doc_http_snippets