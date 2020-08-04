import googlemaps
import time
from bs4 import BeautifulSoup

# Define the API Key.
API_KEY = 'eneter key'

# Define the Client
gmaps = googlemaps.Client(key = API_KEY)

# Do a simple nearby search where we specify the location
# in lat/lon format, along with a radius measured in meters

def get_hospial(location,rad):
    try:
        places_result = gmaps.places_nearby(location=location, radius=rad, open_now=False, type='hospital')
        time.sleep(1.75)
        place_result = gmaps.places_nearby(page_token=places_result['next_page_token'])
    except:
        places_result=get_hospial(location,rad+2000)
    return places_result

def request_handler(request):
    location = request['form']['loc']
    stored_results = []
    # loop through each of the places in the results, and get the place details.
    places_result=get_hospial(location,8000)
    for place in places_result['results']:

        # define the place id, needed to get place details. Formatted as a string.
        my_place_id = place['place_id']

        # define the fields you would liked return. Formatted as a list.
        my_fields = ['name','formatted_phone_number','adr_address']

        # make a request for the details.
        places_details  = gmaps.place(place_id= my_place_id , fields= my_fields)

        # store the results in a list object.
        stored_results=(places_details['result'])
        break

    for key in stored_results.keys():
        stored_results[key]= BeautifulSoup(stored_results[key], 'html.parser').text

    return (stored_results)
