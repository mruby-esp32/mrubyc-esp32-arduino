puts "mruby/c example: control LED"
Arduino.pin_mode(4,:OUTPUT)
while true
	Arduino.digital_write(4,:HIGH)
	sleep(1)
	Arduino.digital_write(4,:LOW)
	sleep(1)
end

