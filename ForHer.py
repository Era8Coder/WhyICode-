import re
import requests
import warnings
from urllib3.exceptions import InsecureRequestWarning
from bs4 import BeautifulSoup
import threading

# Suppress only the InsecureRequestWarning
warnings.simplefilter('ignore', InsecureRequestWarning)

# Define the URL of the page to parse
url = 'https://ldapweb.iitd.ac.in/LDAP/courses/'

# Define the regex pattern
pattern = r'2401-[A-Za-z]{3}\d{3}\.shtml'

# take input the name
n = int(input())
names = []
for i in range(n):    
    name = input("Please enter the name: ")
    names.append(name)

# Fetch the webpage
response = requests.get(url, verify=False)
response.raise_for_status()  # Raise an exception for HTTP errors

# Parse the HTML content
soup = BeautifulSoup(response.text, 'html.parser')

# Find all links in the page
links = soup.find_all('a', href=True)

valid_links = [link for link in links if re.search(pattern, link['href'])]

# Function to process a chunk of links
def process_links(link_chunk):
    for link in link_chunk:
        href = link['href']
        full_url = requests.compat.urljoin(url, href)
        try:
            response = requests.get(full_url, verify=False)
            response.raise_for_status()
            soup = BeautifulSoup(response.text, 'html.parser')
            for name in names:
                if name in soup.text:
                    print(f"Name: {name} Found the link: {full_url}")
        except requests.RequestException as e:
            print(f"Request failed for {full_url}: {e}")

# Split valid_links into chunks of 50
chunk_size = 50
chunks = [valid_links[i:i + chunk_size] for i in range(0, len(valid_links), chunk_size)]

# Create and start a thread for each chunk
threads = []
for chunk in chunks:
    thread = threading.Thread(target=process_links, args=(chunk,))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()