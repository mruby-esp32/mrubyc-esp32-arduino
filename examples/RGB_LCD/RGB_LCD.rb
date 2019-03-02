lcd = RGB_LCD.new
n=0
while true do
  lcd.clear
  lcd.set_cursor(0,0)
  lcd.write "Count #{n}"
  n+=1 if n<100
  lcd.set_rgb((n*10)%250,(n*10)%250,(n*10)%250)
  Arduino.delay 1000
end
