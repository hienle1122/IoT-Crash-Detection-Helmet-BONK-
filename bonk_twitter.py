import tweepy
import sqlite3
import googlemaps
import time
from bs4 import BeautifulSoup

tweet_detail_db = '__HOME__/twitter/twitter.db'  # database to store mapping of user to emergency contact

twitter_API_key = "YcxZeH1S2mdgxv0iQNAybjpr3"
twitter_API_secret_key = "KwkaEnBYzlTZ9kgysyBQHHsVW3wLI77PwYncy8xFSSN0LdQbvM"

twitter_Access_token = "1248397532339810304-dZrnUa2hUZa8XnIJm2VFxAidF8lxxl"
twitter_Access_secret_token = "4XtVtSdeNytodFIWXo1NQOcbrtxke0iC33t23nofDyGZx"

auth = tweepy.OAuthHandler(twitter_API_key, twitter_API_secret_key)
auth.set_access_token(twitter_Access_token, twitter_Access_secret_token)

# The central account doing all the posting/sending
id_aka_handle = "BBBonk_"
user_id = "BONK™"

api = tweepy.API(auth)

Google_API_KEY = 'AIzaSyCs-oYxiui65Y1ZSebjkZVRpyq-Z-5qdBg'

gmaps = googlemaps.Client(key=Google_API_KEY)


def nearest_hospital(location, radius=8000):
    """
    :param location: string of coordinates
    :return: dictionary of nearest hospitals informatino
    example -> {'adr_address': '21214 Northwest Fwy, Cypress, TX 77429, USA', 'formatted_phone_number': '(832) 912-3500', 'name': 'HCA Houston Healthcare North Cypress'}
    """
    places_result = gmaps.places_nearby(location=location, radius=radius, open_now=False, type='hospital')
    time.sleep(1.75)
    place_result = gmaps.places_nearby(page_token=places_result['next_page_token'])
    stored_results = []
    # loop through each of the places in the results, and get the place details.

    for place in places_result['results']:
        # define the place id, needed to get place details. Formatted as a string.
        my_place_id = place['place_id']

        # define the fields you would liked return. Formatted as a list.
        my_fields = ['name', 'formatted_phone_number', 'adr_address']

        # make a request for the details.
        places_details = gmaps.place(place_id=my_place_id, fields=my_fields)

        # store the results in a list object.
        stored_results = (places_details['result'])
        break

    for key in stored_results.keys():
        stored_results[key] = BeautifulSoup(stored_results[key], 'html.parser').text

    return stored_results


def bonk_timeline():  # prints out all tweets on bonk's timeline
    public_tweets = api.home_timeline()
    for tweet in public_tweets:
        print(tweet.text)


def get_info(handle):  # code for getting all information about a certain account
    bonk = api.get_user(handle)
    return bonk


def get_user_tweets(handle, count=200):  # can only get 200 most recent tweets
    """
    :param handle:  string of a certain twitter handle (ie: @BBBonk_)
    :return: list of that handle's most recent tweets tweets after processing/cleaning
    """
    tweets = api.user_timeline(handle)
    cleaned_tweets = []
    for i in range(len(tweets)):
        tweet = tweets[i]._json['text']
        cleaned_tweets.append(tweet)
    return cleaned_tweets[:count]


def post_tweet(tweet, coordinate=None):
    """
    :param tweet: string containing what is about to be tweeted
    :param coordinate: tuple containing lat and long of where the tweet is being tweeted
    :return: None
    """
    if coordinate is None:
        api.update_status(tweet)
    else:
        api.update_status(tweet, lat=coordinate[0], long=coordinate[1])


def send_dm(user_id, message):
    """
    :param user_id: a long integer that connects a certain handle to ID
    :param message: string of what you are saying to them
    :return: send the dm from the bonk account
    """
    api.send_direct_message(user_id, message)


def request_handler(request):
    conn = sqlite3.connect(tweet_detail_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    if request['method'] == 'GET':
        return "GET requests are not allowed. Please try sending a POST request."
    if request['method'] == 'POST':
        action = request['form']['status']  # string that tells us what tweet to post (serious, funny, or dm)
        handle = request['form']['user']  # string that has the twitter handle (ie: @BBBonk_) without the @
        location = request['form']['loc']  # string of coordinates from the wifi sniffer
        comma = location.rfind(',')  # finds the index of the  comma in the location string(where lat and long are separated)
        if action == 'funny':  # if action is funny then we know the user is ok and we can make fun of them with a funny tweet
            tweet = 'HAHAHHA @' + handle + ' fell off their skateboard. And their shorts ripped too lol. Tough day.'
            if comma != -1:  # that means a location was sent in
                coordinate = (float(location[:comma]), float(location[comma + 1:]))  # stores the lat and long as floats (used for tweet location tag)
                post_tweet(tweet, coordinate)
            else:
                post_tweet(tweet)
        elif action == 'danger':  # if action is danger then the user might be in serious pain or gotten hurt badly so don't make fun of them
            if comma != -1:  # that means a location was sent in
                coordinate = (float(location[:comma]), float(location[comma + 1:]))  # stores the lat and long as floats (used for tweet location tag)
                try:
                    hospital_info_dict = nearest_hospital(location, radius=8000)
                    hospital_addy = hospital_info_dict['adr_address']  # get the address of the hospital
                    comma = hospital_addy.rfind(',')
                    hospital_addy = hospital_addy[:comma]
                    space = hospital_addy.rfind(' ')  # index of last whitespace
                    hospital_name = hospital_info_dict['name']
                    if space != -1:  # checks if there was an extra whitespace
                        hospital_addy = hospital_addy[:space]  # removes any possible whitespace
                    tweet = 'Oh noo, @' + handle + ' has fallen badly and needs your help. Check up on them if you can. If they need medical help: the closest hospital is ' + hospital_name + ' at ' + hospital_addy + '.'
                    post_tweet(tweet, coordinate)
                except:
                    try:  # double the search radius for finding a hospital if the tweet failed
                        hospital_info_dict = nearest_hospital(location, radius=16000)
                        hospital_addy = hospital_info_dict['adr_address']  # get the address of the hospital
                        comma = hospital_addy.rfind( ',')
                        hospital_addy = hospital_addy[:comma]
                        space = hospital_addy.rfind(' ')  # index of last whitespace
                        hospital_name = hospital_info_dict['name']
                        if space != -1:  # checks if there was an extra whitespace
                            hospital_addy = hospital_addy[:space]  # removes any possible whitespace
                        tweet = 'Oh noo, @' + handle + ' has fallen badly and needs your help. Check up on them if you can. If they need medical help: the closest hospital is ' + hospital_name + ' at ' + hospital_addy + '.'
                        post_tweet(tweet, coordinate)
                    except:  # if search fails twice then just tweet they need help and no available hospitals could be found
                        tweet = 'Oh noo, @' + handle + ' has fallen badly and needs your help. Check up on them if you can. We could not find any available hospitals nearby. Perhaps try looking for an ER if needed.'
                        post_tweet(tweet, coordinate)
            else:
                tweet = 'Oh noo, @' + handle + ' has fallen badly and needs your help. Check up on them if you can. We could not find any available hospitals nearby. Perhaps try looking for an ER if needed.'
                post_tweet(tweet)

        elif action == 'longtime':  # if its been a longtime after danger and the user still hasn't moved (presumably no help has come or checked in) send a dm to their emergency contact
            message = 'Hi, you were chosen as @'+ handle + '\'s emergency contact if they ever got hurt/fell while skating and using Bonk. You are getting this message because they have fallen and have not gotten up yet. Please check up on them, visit our profile to for a tweet with details concerning their location and the nearest Hospital. (This message is automated).'
            who_to_dm = c.execute('''SELECT contact FROM emergency_table WHERE user = ?;''', (handle,)).fetchall()  # gets the emergency contact for this user
            conn.commit()  # commit commands
            conn.close()  # close connection to database
            who_to_dm = set(who_to_dm)  # set of the form {('RomiFarmCash',), ('harshal13c',), ('tchanceh5',)}
            len_who = len(who_to_dm)
            failed = []
            for person in who_to_dm:  # person is of the form ('RomiFarmCash',)
                try:
                    info = get_info(person[0])._json
                    user_id = info['id']
                    send_dm(user_id, message)  # sends the dm to the emergency contact
                except:
                    failed.append(person[0])  # means there was an error when trying to send a dm (usually they dont follow bonk)
            if len(failed) > (len_who / 2):  # if more than half of our emergency contact list did not receive a dm then @ them in a public tweet
                people_at = ''
                for index, user in enumerate(failed):
                    if index < 6:  # only @ a maximum of 5 users (twitter has a cap of 240 chars per tweet dont want to break that)
                        people_at += '@'+user+' '
                tweet = people_at + ' Please checkup on @' + handle +', they may be injured or hurt. We tried to contact you through direct message but we could not reach you. Check our timeline for more information.'
                post_tweet(tweet)