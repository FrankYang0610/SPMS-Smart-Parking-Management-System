import random

# Define the user names, command types, and essential devices
user_names = ['member_A', 'member_B', 'member_C', 'member_D', 'member_E']
command_types = ['addParking', 'addReservation', 'addEvent', 'bookEssentials']
essential_devices = [['battery', 'cable'], ['locker', 'umbrella'],['InflationService', 'valetPark']]

# Function to generate a random date and time within the specified range
def generate_random_datetime():
    year = 2025
    month = 5
    day = random.randint(10, 16)
    hour = random.randint(0, 23)
    minute = 0  # As mentioned, minute is fixed to 0 for every hour slot
    return f"{year:04d}-{month:02d}-{day:02d} {hour:02d}:{minute:02d}"

# Function to generate a random duration in hours (integer)
def generate_random_duration():
    return float(random.randint(1, 24))

# Function to generate random essential devices (if any)
def generate_random_essentials(command):
    if command == 'addReservation' or 'bookEssentials': return random.choice(essential_devices)
    if random.choice([True, False]):
        return random.choice(essential_devices)
    else:
        return None

# Generate 1000 random instructions
instructions = []
N = 3000
for _ in range(N):
    user_name = random.choice(user_names)
    command_type = random.choice(command_types)
    datetime = generate_random_datetime()
    duration = generate_random_duration()
    essentials = generate_random_essentials(command_type)
    
    if essentials:
        if command_type == 'bookEssentials':
            e = random.choice([0,1])
            instruction = f"{command_type} -{user_name} {datetime} {duration} {essentials[e]};"

        else:
            instruction = f"{command_type} -{user_name} {datetime} {duration} {essentials[0]} {essentials[1]};"
    else:
        instruction = f"{command_type} -{user_name} {datetime} {duration};"
    
    instructions.append(instruction)

# Write the instructions to a dat file
with open('batch001.dat', 'w') as file:
    for i, instruction in enumerate(instructions):
        if i == len(instructions) - 1:
            file.write(instruction)
        else:
            file.write(instruction + '\n') 

print(f"{N} instructions have been generated and saved in 'batch001.dat'.")
