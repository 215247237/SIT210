from tkinter import*
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

# LED setup
living_room_led = 18       
bathroom_led = 27
closet_led = 22

GPIO.setup(living_room_led, GPIO.OUT, initial = GPIO.LOW)      
GPIO.setup(bathroom_led, GPIO.OUT, initial = GPIO.LOW)
GPIO.setup(closet_led, GPIO.OUT, initial = GPIO.LOW)

# PWM setup for living room LED                             
living_room_pwm = GPIO.PWM(living_room_led, 1000)
living_room_pwm.start(0)

# GUI setup
gui_window = Tk()
gui_window.title("Linda's Lights")
gui_window.geometry("400x250")                         
selected_room = IntVar()

# Function to handle radio button design
def button_design(button_text, room_led, bg_colour):
    Radiobutton(gui_window, text = button_text, font = ("Helvetica", 16),      
        variable = selected_room, value = room_led,
        command = lambda: toggle_led(room_led), bg = bg_colour
        ).pack(anchor = "w", fill = "x", padx = 8, pady = 5)

# Function to turn ALL LEDs off
def turn_off_leds():
    living_room_pwm.ChangeDutyCycle(0)                                   
    GPIO.output(bathroom_led, GPIO.LOW)
    GPIO.output(closet_led, GPIO.LOW)

# Function to turn all LEDs off and turn selected LED on
def toggle_led(room_led):
    turn_off_leds()
    
    if room_led == living_room_led:
        living_room_pwm.ChangeDutyCycle(100)                              
    else:
        GPIO.output(room_led, GPIO.HIGH)

# Function that defines exit button to close GUI
def exit_button():
    living_room_pwm.stop()                                    
    GPIO.cleanup()
    gui_window.destroy()

# Function that defines slider to control LED brightness       
def brightness_slider(slider_value):
    brightness = float(slider_value)
    
    if selected_room.get() == living_room_led:
        living_room_pwm.ChangeDutyCycle(brightness)

# Create window label
Label(gui_window, text = "Linda's Lights", font = ("Helvetica", 20)).pack()  

# Create living room radio button and slider
button_design("Turn living room light ON", living_room_led, "DeepSkyBlue1")    
Label(gui_window, text = "Adjust brightness", font = ("Helvetica", 13), bg = "DeepSkyBlue1").pack()
Scale(gui_window, command = brightness_slider, from_ = 0, to = 100, orient = HORIZONTAL,
    length = 250, bg = "DeepSkyBlue1").pack(anchor = CENTER)    

# Create bathroom and closet radio buttons
button_design("Turn bathroom light ON", bathroom_led, "gold1")         
button_design("Turn closet light ON", closet_led, "IndianRed1")

# Create exit button
Button(gui_window, text = "Exit", font = ("Helvetica", 16),
    command = exit_button, height = 1, width = 10).pack()

gui_window.protocol("WM_DELETE_WINDOW", exit_button)
gui_window.mainloop()