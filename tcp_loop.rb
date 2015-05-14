#!/usr/bin/env ruby 

require 'socket'

host = "localhost"
port = ARGV[0].to_i or 8080

puts "Port: #{port}"

(0..2000).each do |index|
  s = TCPSocket.open(host, port)
  printf "#{index} : echo ... "
  s.puts "echo"
  printf s.gets
  puts "\n"
  s.close
end
