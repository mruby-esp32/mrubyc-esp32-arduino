#avatar = M5Avatar.new

kazu = ["ichi","ni","san","yon","go"]
#avatar.speech("kazuwokazoeruyo")
#Arduino.delay(3000)
puts "kazoeru"
#Arduino.delay(1000)

kazu.each do |n|
#while true
  puts ">"
  puts n
  #avatar.speech(n)
  Arduino.delay(2000)
end
