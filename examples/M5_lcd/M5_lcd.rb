puts "mruby/c example: M5 Lcd"

#Lcd display
Lcd.fill_screen(:WHITE)
sleep(1)
Lcd.fill_screen(:RED)
sleep(1)
Lcd.fill_screen(:GREEN)
sleep(1)
Lcd.fill_screen(:BLUE)
sleep(1)
Lcd.fill_screen(:BLACK)
sleep(1)

#text print
puts "mruby/c example: M5 Lcd text print"
Lcd.fill_screen(:BLACK)
Lcd.set_cursor(10, 10)
Lcd.set_text_color(:WHITE)
Lcd.set_text_size(1)
w = Lcd.width
h = Lcd.height
Lcd.printf("Display Test! width=#{w} height=#{h}")

#draw graphic
puts "mruby/c example: M5 Lcd draw graphic"
sleep(1)
Lcd.draw_rect(100, 100, 50, 50, :BLUE)
sleep(1)
Lcd.fill_rect(100, 100, 50, 50, :BLUE)
sleep(1)
Lcd.draw_circle(100, 100, 50, :RED)
sleep(1)
Lcd.fill_circle(100, 100, 50, :RED)
sleep(1)
Lcd.draw_triangle(30, 30, 180, 100, 80, 150, :YELLOW)
sleep(1)
Lcd.fill_triangle(30, 30, 180, 100, 80, 150, :YELLOW)

puts "mruby/c example: M5 Lcd loop"

while true
	Lcd.fill_triangle(Arduino.random(w-1), Arduino.random(h-1), \
		Arduino.random(w-1), Arduino.random(h-1), \
		Arduino.random(w-1), Arduino.random(h-1), Arduino.random(0xfffe))
	M5.update();
end

