
# BONK

<a href="https://drive.google.com/uc?export=view&id=1ak-aZIXE4k-AAuWupSbL0l8T-EJKmeBz">
<img src="https://drive.google.com/uc?export=view&id=1ak-aZIXE4k-AAuWupSbL0l8T-EJKmeBz"
style="width: 500px; max-width: 100%; height: auto"
title="Click for the larger version." />
</a>
<div align="left">

<h1> Intro </h1>

[![Bonk Demo](https://yt-embed.herokuapp.com/embed?v=vU2uWvjclJM)](https://www.youtube.com/watch?v=vU2uWvjclJM "BONK Demo")

<h1>
    What the system does
</h1>
    <p>
        What BONK&#8482; does is that a tweet will be posted when the system detects the user falling.
        The tweet will try to include as many relevant details as possible about the user and the fall.
        This can serve as the basis for a unique safety system. If the user falls but the system detects
        that they have gotten up within 30 seconds, a funny tweet about the user will be posted. However,
        if the user had fallen badly, then a serious tweet containing where the user fell and the nearest
        hospital&#39;s address will be posted. Additionally, if no movement is detected for a longtime (no
        movement after fall for more than 30 seconds) then direct messages will be attempted to be sent to
        everyone on that user&#39;s emergency contact list. If more than half of the contacts cannot be reached
        through direct message (usually due to their privacy settings), then a public tweet will be posted
        mentioning them.
    </p>
    <div align="center">
    <a href="https://drive.google.com/uc?export=view&id=1hTbjG1bmqQ_ybexEzNaCy1E3G19A2XaM">
    <img src="https://drive.google.com/uc?export=view&id=1hTbjG1bmqQ_ybexEzNaCy1E3G19A2XaM"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1LlaguBRhTQb4iIx3HpKJqms1jQJoCqsL">
    <img src="https://drive.google.com/uc?export=view&id=1LlaguBRhTQb4iIx3HpKJqms1jQJoCqsL"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1hUhn1Ao8yfG3Qf1RP_OVN8XbCBUT_YCD">
    <img src="https://drive.google.com/uc?export=view&id=1hUhn1Ao8yfG3Qf1RP_OVN8XbCBUT_YCD"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1QWoxThbsQFAS1ElTnZH4W3d3nE4VbGdg">
    <img src="https://drive.google.com/uc?export=view&id=1QWoxThbsQFAS1ElTnZH4W3d3nE4VbGdg"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    </div>
<h1>
   How it does it
</h1>
<h2>
    Software side
</h2>
<h3>
    ESP32/FSM
</h3>
    <p>
        The code on the ESP32 mainly consists of a state machine where all of the fall and location
        detection takes place. The diagram below is a graphical explanation of how the code works.
    </p>
    <div align="center">
    <a href="https://drive.google.com/uc?export=view&id=1mPcbwkjeZqyMnQQstqnzwhFYUjNJLapb">
    <img src="https://drive.google.com/uc?export=view&id=1mPcbwkjeZqyMnQQstqnzwhFYUjNJLapb"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    </div>
<h3>
    Google maps
</h3>
    <p>
        Because it was very hard for us to reliably get a clear signal with the GPS module, we decided to
        implement the ESP32&#39;s ability to scan for available wifi networks, along with a Google
        WifiLocations library to get us the coordinates that we needed for everything else to function
        correctly. We start off by telling the ESP32 look around it for nearby Wifi signals as well as
        the strength of these signals. The WiFi Locations library then sends this information up to
        GoogleMaps API. The GoogleMaps API in turn responds with a packet of information containing
        things like longitude, latitude as well as the accuracy of location. This is the only way we
        found that consistently got a good signal for our coordinates.
    </p>
    <p>
        If a fall is detected by the system. We can send the GPS coordinates that were returned to us from the
        Wifi sniffing to the server. The server then communicates with Google Maps to look for a nearby hospital.
        If it cannot find one within the initial radius, it will recursively increase the radius of the search
        until one is found (or until the server times out). When it finds a hospital, it will parse through the
        information that Google returned and get the useful information to the user in a form of a dictionary.
        In the last step before we send the dictionary back to the arduino, we use BeautifulSoup to clean up all
        the html tags. Twitter side of the code then uses this information to tweet things according to the
        situation.
    </p>
<h3>
    Tweeting
</h3>
    <p>
        All of the Twitter functionality is done through the use of the Twitter developer API
        (the BONK&#8482; account is registered as a developer) and a python library called tweepy, which makes
        accessing the API much easier and efficient. The database used to store a user’s twitter handle
        and their respective contact list (one column for the user and another column for their contacts).
        Furthermore, depending on if the GPS module or wifi sniffer found a location and which part of the
        state machine the system is in affects how the server responds. Additionally, when first setting up
        BONK&#8482; there is a file on the server that maps a user&#39;s twitter handle to their emergency contacts&#39;
        twitter handles and then stores that into a database that is used for the direct message feature of BONK&#8482;.
        Once the server has the location (as a coordinate) and the user&#39;s twitter handle, it will use the
        google maps API to find the nearest hospital to the user. If no hospital can be found in the starting
        radius, then the server will try finding the hospital with double the starting radius. If doubling the
        radius still fails to find a hospital then the server will post the tweet without the hospital information.
        When the direct messages are being sent, the server looks up the user&#39;s emergency contact list in
        the database.
    </p>
    <div align="center">
    <a href="https://drive.google.com/uc?export=view&id=1rE9wL9aHgQw1yBetOEijbW7lD3E_kyGt">
    <img src="https://drive.google.com/uc?export=view&id=1rE9wL9aHgQw1yBetOEijbW7lD3E_kyGt"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    </div>
<h2>
    Hardware side
</h2>
<h3>
    Power
</h3>
    <p>
        The ESP32 is in series with all of the other components, which includes the IMU, GPS, and battery
        pack. The battery pack is able to be charged still from its soldered micro-USB port and is set up
        to provide power to the ESP32&#39;s 3.3V power pins as well as ground to ground.
    </p>
    <div align="center">
    <a href="https://drive.google.com/uc?export=view&id=1kgMeho3UJuz5-DIPIiGyDcoMQMLK3Suu">
    <img src="https://drive.google.com/uc?export=view&id=1kgMeho3UJuz5-DIPIiGyDcoMQMLK3Suu"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1Ar6wnf98sW3gIGlxAOJFe4o2ZWf9fcgr">
    <img src="https://drive.google.com/uc?export=view&id=1Ar6wnf98sW3gIGlxAOJFe4o2ZWf9fcgr"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=11k7wQ-c8BHPqEpFLy-Izj2dXbKO7BqHW">
    <img src="https://drive.google.com/uc?export=view&id=11k7wQ-c8BHPqEpFLy-Izj2dXbKO7BqHW"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1RN_0_gxSUImSPpWhdWaP16Zva82-8Cfe">
    <img src="https://drive.google.com/uc?export=view&id=1RN_0_gxSUImSPpWhdWaP16Zva82-8Cfe"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    </div>

<h3>
    Fall Detection
</h3>
    <p>
        Fall detection was mainly implemented by using an IMU to measure the acceleration of the helmet. We
        monitor the readings coming from IMU and if we see a value that is higher than what we consider normal,
        then a fall is recorded. The challenge with this was figuring out what a good IMU reading threshold would
        be for detecting a fall. We began by using the Arduino IDE&#39;s Serial Plotter to see the acceleration
        values that were coming out of the IMU. Testing was conducted by doing normal activities like walking
        or riding a skateboard around and seeing what the readings looked like. We wanted to make sure we knew
        what a fall did NOT look like so that we could better identify what a fall DID look like later on.
        For falls, we dropped the helmet on the ground or jumped on to a mattress in order to get an idea for
        what readings look like when you fall. We ran into the problem of not being able to accurately read
        high acceleration values because the library we were using to interface with the IMU had the sensitivity
        of the accelerometer set to 2 G&#39;s.  After switching the sensitivity of the IMU to be able to handle
        up to 8 G&#39;s, we were able to see the difference in readings between falls and normal movements such
        as riding a skateboard.
    </p>

<h1>
    Why it was designed this way
</h1>
<h2>
    Power consumption
</h2>
    <p>
        In order to save power, the ESP32 alternates between &#34;Active mode&#34; and &#34;Deep Sleep&#34; mode. The
        difference is that in Active mode, the ESP32 uses at max 260 mA of current while in Deep sleep,
        it only uses 10 uA of current, which is 26000 times smaller of a current, meaning it saves about
        26000 times as much energy. Just for reference, if the ESP32 were not to be put into Deep Sleep,
        then the whole computer would only last for about .25 of a day, which is much too short for our
        IOT helmet.
    </p>
    <p>
        There are 3 main modes for the ESP32: Active, Modem, and Deep sleep modes. The reason we chose to put
        the ESP32 into Deep sleep is because the Modem mode does save some energy, but not enough to make it
        worth it. If the ESP32 is going to have all of this idle time, then we need to have it save as much
        energy as possible. With this being said, there are only three methods to wake up an ESP32 from Deep
        sleep: touching two pins together, an external signal, and with a timer. Since this needs to be an
        IOT helmet with as little human interaction as possible, the timer needs to be used. The way the
        sleep finite state machine is set up in Arduino is that it goes to sleep for 30 seconds if the IMU
        measurement is below a certain threshold, then it wakes up and the cycle repeats. With this system,
        the ESP32 is able to stay on for about 1.8 days with it being used for about 90 minutes out of the day.
    </p>
    <p>
        How these values were calculated were from summing the power usages from the IMU, the GPS, and the
        ESP32 and then calculating for the active time. Two ways these figures could be improved would 1)
        put the IMU into its own sleep mode, which can be figured out through the registry, and 2) increasing
        the size of the battery. Because BONK&#8482; needs to be charged about every other day, this already exceeds
        our expectations in terms of power consumption, but if it wanted to be extended, these methods can be
        used. Also, the battery used in the battery pack is an 18650, which is just the size of the battery,
        and I&#39;ve seen capacities go up to 4000 mAh while ours is only 1500 mAh.
    </p>
    <div align="center">
    <a href="https://drive.google.com/uc?export=view&id=1VwAIBixMWSbAyq9GWVg_gZEpEuOM0vsq">
    <img src="https://drive.google.com/uc?export=view&id=1VwAIBixMWSbAyq9GWVg_gZEpEuOM0vsq"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1r39EKxoFoKoU2ybu_TEaKuO58e7yWMj7">
    <img src="https://drive.google.com/uc?export=view&id=1r39EKxoFoKoU2ybu_TEaKuO58e7yWMj7"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    <a href="https://drive.google.com/uc?export=view&id=1WwanKqcTYbDfQjiTqIEoOQ170BhXqRCV">
    <img src="https://drive.google.com/uc?export=view&id=1WwanKqcTYbDfQjiTqIEoOQ170BhXqRCV"
    style="width: 500px; max-width: 100%; height: auto"
    title="Click for the larger version." />
    </a>
    </div>
<h2>
    Direct Messages
</h2>
    <p>
        The idea behind the emergency contact feature is that a user can set up an emergency contact(s)
        (with their twitter handle) to be notified if/when they fall or get injured badly and need someone&#39;s
        help. For example, if I were riding to class and fell down, but got back up relatively easily with only
        a scratch or two then there would be no need for a serious tweet with my location and that I fell.
        However, if I had fallen really badly and need help but nobody who sees the tweet really cares
        (or they just scroll too fast to care), a direct message will be sent to my &#34;emergency contacts&#34;
        (people I think would care enough to check up on me). Hopefully by sending that person a direct
        message they will notice and actually help. Having this be handled by the server is advantageous
        since presumably one might not be in the right state of mind to try and contact someone for help if
        they had fallen extremely badly.
    </p>

