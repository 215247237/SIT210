from tkinter import*
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

# LED setup
living_room_led = 17
bathroom_led = 27
closet_led = 22

GPIO.setup(living_room_led, GPIO.OUT, initial = GPIO.LOW)
GPIO.setup(bathroom_led, GPIO.OUT, initial = GPIO.LOW)
GPIO.setup(closet_led, GPIO.OUT, initial = GPIO.LOW)

# GUI setup
gui_window = Tk()
gui_window.title("Linda's lights")
gui_window.geometry("400x200")
selected_room = IntVar()

# Function to handle radio button design
def button_design(button_text, room_led, bg_colour):
    Radiobutton(gui_window, text = button_text, font = ("Helvetica", 20),
        variable = selected_room, value = room_led,
        command = lambda: toggle_led(room_led), bg = bg_colour)
        .pack(anchor = "w", fill = "x", padx = 8, pady = 5)

# Function to turn ALL LEDs off
def turn_off_leds():
    GPIO.output(living_room_led, GPIO.LOW)
    GPIO.output(bathroom_led, GPIO.LOW)
    GPIO.output(closet_led, GPIO.LOW)

# Function to turn all LEDs off and turn selected LED on
def toggle_led(room_led):
    turn_off_leds()
    GPIO.output(room_led, GPIO.HIGH)

# Function that defines exit button to close GUI
def exit_button():
    GPIO.cleanup()
    gui_window.destroy()

# Create radio buttons
button_design("Living room light", living_room_led, "DeepSkyBlue1")
button_design("Bathroom light", bathroom_led, "gold1")
button_design("Closet light", closet_led, "IndianRed1")

# Create exit button
Button(gui_window, text = "Exit", font = ("Helvetica, 18"),
    command = exit_button, height = 2, width = 15).pack()

gui_window.protocol("WM_DELETE_WINDOW", exit_button)
gui_window.mainloop()