"""
Get yourself a cat fact over 4G!
"""

import lte
import time
import requests

MOBILE_APN = "iot.1nce.net"

con = lte.LTE(MOBILE_APN)
con.start_ppp()

try:
    t_start = time.time()
    request = requests.get("http://catfact.ninja/fact").json()
    fact = request["fact"]
    print("Cat fact!")
    print(fact)

finally:
    t_end = time.time()

    print(f"Took: {t_end - t_start} seconds")

    print("Disconnecting...")
    con.stop_ppp()
    print("Done!")
