"""
extract_graph.py — Delhi NCR Road + Transit Graph Extractor (OSMnx)
════════════════════════════════════════════════════════════════════

Downloads real road network data from OpenStreetMap using osmnx,
and saves it as data/delhi_ncr.json for the C++ routing engine.

Usage:
  cd scripts
  python extract_graph.py

Dependencies:
  pip install osmnx networkx pandas
"""

import osmnx as ox
import networkx as nx
import json
import os

print("Fetching Delhi-NCR transit & road network from OpenStreetMap...")

# Delhi ka bounding box ya place name specify karo (e.g., Central/New Delhi core corridor)
place_query = "New Delhi, India"
G = ox.graph_from_place(place_query, network_type='drive', simplify=True)

print(f"Graph downloaded! Nodes: {G.number_of_nodes()}, Edges: {G.number_of_edges()}")

# NetworkX graph ko nodes aur edges ki dictionaries mein tod kar JSON banate hain
nodes_data = {}
for node, data in G.nodes(data=True):
    nodes_data[node] = {
        "lat": data.get("y"),
        "lon": data.get("x")
    }

edges_data = []
for u, v, data in G.edges(keys=False, data=True):
    # Basic attributes: distance (meters), maxspeed, road name
    length = data.get("length", 1.0)
    maxspeed = data.get("maxspeed", 30)  # default speed assumption if not tagged

    # Agar maxspeed string hai (e.g., "50 km/h"), usko integer mein parse karo
    if isinstance(maxspeed, list):
        maxspeed = maxspeed[0]
    if isinstance(maxspeed, str):
        try:
            maxspeed = int(maxspeed.split()[0])
        except:
            maxspeed = 30

    edges_data.append({
        "u": u,
        "v": v,
        "length": length,
        "speed": maxspeed
    })

graph_json = {
    "nodes": nodes_data,
    "edges": edges_data
}

# Data folder mein save karo
output_path = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "data", "delhi_ncr.json")
os.makedirs(os.path.dirname(output_path), exist_ok=True)

with open(output_path, "w") as f:
    json.dump(graph_json, f)

print(f"Successfully saved local graph to {output_path}!")
print(f"  Nodes: {len(nodes_data)}")
print(f"  Edges: {len(edges_data)}")
