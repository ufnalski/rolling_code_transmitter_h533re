# Rolling/hopping code authentication demo (transmitter) [STM32H533RE]
Inspired by the "unhackable" modern steering column locks such as [3Q0905861A](https://allegro.pl/listing?string=3Q0905861A) (Audi A4 B9). The content of my repos makes it pretty obvious that I'm currently on the hunt for every (servo) drive from a passenger car. This time the hunter becomes the hunted. Steering columns locks will hunt me to the end of 2025 - two more weeks left :slightly_smiling_face: I will probably fail miserably. There are at least three reasons for that. First, the extended CAN ID renders brute-force search for IDs impractical for a DUT without a working car. Second, an electronic steering column lock (ESCL) is a security feature designed to prevent unauthorized use of the vehicle - do not expect it to react on a single CAN frame with a fixed payload. Third, I'm not a cybersecurity hobbyist (yet) and my tools are close to nonexistent in this particular field. If you are in a similar position, my suggestion is to start from something less challenging. Let's build our own transmitter and receiver pair that uses rolling code authentication. Are you curious upon what principle a remote keyless entry (RKE) system may be based? If yes, you came to the right place.

![Audi A4 B9 ESCL](/Assets/Images/audi_a4_b9_escl.jpg)
![Audi A4 B9 ESCL can frames](/Assets/Images/steering_column_lock_can_frames.JPG)
![Rolling codes in action](/Assets/Images/rolling_codes_in_action.jpg)
![Rolling codes can frames](/Assets/Images/rolling_codes_trace.JPG)
![Rolling codes in action](/Assets/Images/receiver_serial_monitor.JPG)

> [!NOTE]
> The demo uses wired communication. We are here to grasp the idea of a hopping code authentication method - the physical layer does not affect our current experiment. You can use any hardware of your choice - preferably the one that offers hardware hashing but you can always do that part also in software[^1] (buttons are pressed by humans relatively slowly). In my case the [receiving device](https://github.com/ufnalski/rolling_code_receiver_h533re) is a compatible CAN bus node.

[^1]: An exemplary use of [Mbed TLS](https://github.com/Mbed-TLS/mbedtls) is demonstrated in my [Wall of Entropy](https://github.com/ufnalski/ov2640_lavarand_h743zi2).

# Missing files?
Don't worry :slightly_smiling_face: Just log in to MyST and hit Alt-K to generate /Drivers/CMCIS/ and /Drivers/STM32H5xx_HAL_Driver/ based on the .ioc file. After a couple of seconds your project will be ready for building.

# Readings
* [Rolling code](https://en.wikipedia.org/wiki/Rolling_code) (Wikipedia)
* [Rolling code](https://grokipedia.com/page/Rolling_code) (Grokipedia)
* [KeeLoq](https://en.wikipedia.org/wiki/KeeLoq) (Wikipedia)
* [An Introduction to KeeLoq Code Hopping](https://ww1.microchip.com/downloads/en/appnotes/91002a.pdf) (Microchip)
* [KeeLoq Code Hopping Encoder](https://ww1.microchip.com/downloads/en/devicedoc/21143b.pdf) (Microchip)
* [RollBack & RollJam: Exploring vulnerabilities to strengthen vehicle security](https://vietsol.com.vn/rollback-rolljam-exploring-vulnerabilities-to-strengthen-vehicle-security/) (Vietsol)
* [How does a rolling code work?](https://crypto.stackexchange.com/questions/18311/how-does-a-rolling-code-work) (StackExchange Cryptography)
* [CAN Injection: keyless car theft](https://kentindell.github.io/2023/04/03/can-injection/) (Canis Automotive Labs)
* [Advancing keyless entry/go](https://www.nxp.com/docs/en/brochure/75017275.pdf) (NXP)
* [Samy Kamkar: Automotive security research](https://en.wikipedia.org/wiki/Samy_Kamkar) (Wikipedia)
* [advrc - Advanced Rolling Codes](https://github.com/arongeo/advrc) (arongeo on GitHub)
* [KeeLoq](https://github.com/hadipourh/KeeLoq) (Hosein Hadipour, hadipourh on GitHub)
* [Rolling Code Authentication System](https://github.com/robert-mcdermott/rolling-code-auth) (Robert McDermott, robert-mcdermott on GitHub)

# Call to action
Create your own [home laboratory/workshop/garage](http://ufnalski.edu.pl/control_engineering_for_hobbyists/2025_dzien_popularyzacji_matematyki/Dzien_Popularyzacji_Matematyki_2025.pdf)! Get inspired by [ControllersTech](https://www.youtube.com/@ControllersTech), [DroneBot Workshop](https://www.youtube.com/@Dronebotworkshop), [Andreas Spiess](https://www.youtube.com/@AndreasSpiess), [GreatScott!](https://www.youtube.com/@greatscottlab), [bitluni's lab](https://www.youtube.com/@bitluni), [ElectroBOOM](https://www.youtube.com/@ElectroBOOM), [Phil's Lab](https://www.youtube.com/@PhilsLab), [atomic14](https://www.youtube.com/@atomic14), [That Project](https://www.youtube.com/@ThatProject), [Paul McWhorter](https://www.youtube.com/@paulmcwhorter), [Max Imagination](https://www.youtube.com/@MaxImagination), [Nikodem Bartnik](https://www.youtube.com/@nikodembartnik), [Stuff Made Here](https://www.youtube.com/@StuffMadeHere), [Mario's Ideas](https://www.youtube.com/@marios_ideas), [Aaed Musa](https://www.aaedmusa.com/), and many other professional hobbyists sharing their awesome projects and tutorials! Shout-out/kudos to all of them! Promote [README-driven learning](http://ufnalski.edu.pl/proceedings/sene2025/Ufnalski_PE_formatted_SENE_2025.pdf) :slightly_smiling_face:

> [!WARNING]
> Rolling/hopping codes - do try them at home :exclamation:

210+ challenges to start from: [Control Engineering for Hobbyists at the Warsaw University of Technology](http://ufnalski.edu.pl/control_engineering_for_hobbyists/Control_Engineering_for_Hobbyists_list_of_challenges.pdf).

Stay tuned :sunglasses:
