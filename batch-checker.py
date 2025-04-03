import re
from datetime import datetime, timedelta
from collections import defaultdict

DATE_FORMAT = "%Y-%m-%d"
MIN_TIME = datetime(2025, 5, 10, 0, 0, 0)
MAX_TIME = datetime(2025, 5, 16, 23, 59, 59)
RESOURCES = ['Parking Slot', 'Battery', 'Cable', 'Locker', 'Umbrella', 'Inflation Service', 'Valet Parking']
CAPACITY = {
    'Parking Slot': 10,
    'Battery': 3,
    'Cable': 3,
    'Locker': 3,
    'Umbrella': 3,
    'Inflation Service': 3,
    'Valet Parking': 3
}

def parse_booking_file(file_path):
    members = defaultdict(list)
    current_member = None
    current_record = None
    parsing_devices = False

    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.rstrip('\n')
            # Check if it's a member line
            if line.startswith("Member_") and " has the following bookings:" in line:
                current_member = line.split(' ')[0]
                current_record = None
                parsing_devices = False
                members[current_member] = []
            elif current_member is None:
                continue
            elif line.strip().startswith('==='):
                parsing_devices = False
                current_record = None
            elif not line.strip():
                parsing_devices = False
                current_record = None
            else:
                # Check if it's a booking line
                if re.match(r'^\d{4}-\d{2}-\d{2}', line.lstrip()):
                    parts = list(filter(None, line.split(' ')))  # Split by spaces and remove empty strings
                    if len(parts) < 4:
                        continue  # Invalid line
                    date_str = parts[0]
                    start_time_str = parts[1]
                    end_time_str = parts[2]
                    type_str = parts[3]
                    device_part = ' '.join(parts[4:]) if len(parts) >=5 else ''

                    # Parse start datetime
                    try:
                        start_date = datetime.strptime(date_str, DATE_FORMAT)
                        start_time = datetime.strptime(start_time_str, "%H:%M").time()
                        start_datetime = datetime.combine(start_date, start_time)
                    except ValueError as e:
                        print(f"Error parsing start time: {e} in line: {line}")
                        continue

                    # Parse end datetime
                    try:
                        end_time = datetime.strptime(end_time_str, "%H:%M").time()
                    except ValueError as e:
                        print(f"Error parsing end time: {e} in line: {line}")
                        continue

                    # Handle end datetime (might cross midnight)
                    end_date = start_date
                    end_datetime = datetime.combine(end_date, end_time)
                    if end_datetime <= start_datetime:
                        end_datetime += timedelta(days=1)

                    # Check if times are within allowed range
                    if start_datetime < MIN_TIME or end_datetime > MAX_TIME:
                        raise ValueError(f"Time range {start_datetime} to {end_datetime} is out of bounds (10-16 May 2025)")

                    current_record = {
                        'start': start_datetime,
                        'end': end_datetime,
                        'type': type_str,
                        'devices': [device_part] if device_part else []
                    }
                    members[current_member].append(current_record)
                    parsing_devices = True
                elif parsing_devices and current_record is not None:
                    # Parse device line，可能包含多个空格，取整个非空部分
                    device = line.strip()
                    if device:
                        current_record['devices'].append(device)

    return members

def calculate_usage(bookings):
    usage = defaultdict(lambda: defaultdict(int))
    errors = []

    for member in bookings:
        for booking in bookings[member]:
            start = booking['start']
            end = booking['end']
            booking_type = booking['type']
            devices = booking['devices']

            # Generate all hours this booking spans
            current = start
            hours = []
            while current < end:
                hours.append(current)
                current += timedelta(hours=1)

            # Check each hour and update usage
            for hour_dt in hours:
                # Calculate hour index (0 to 167)
                hour = int((hour_dt - MIN_TIME).total_seconds() // 3600)
                if hour < 0 or hour >= 168:
                    errors.append(f"Invalid hour {hour_dt} (hour index {hour}) in booking {booking}")
                    continue

                # Update Parking Slot usage if applicable
                if booking_type in ['Reservation', 'Parking', 'Event']:
                    usage['Parking Slot'][hour] += 1

                # Update device usage
                for device in devices:
                    if device not in RESOURCES[1:]:  # Skip 'Parking Slot'
                        errors.append(f"Invalid device '{device}' in booking {booking}")
                        continue
                    usage[device][hour] += 1

    return usage, errors

def check_conflicts(usage):
    conflicts = []
    for resource in RESOURCES:
        max_cap = CAPACITY[resource]
        for hour in range(168):
            current = usage[resource].get(hour, 0)
            if current > max_cap:
                start_time = MIN_TIME + timedelta(hours=hour)
                conflicts.append(f"{resource} over capacity at {start_time} ({current} > {max_cap})")
    return conflicts

if __name__ == "__main__":
    try:
        members = parse_booking_file('file.txt')
        usage, errors = calculate_usage(members)
        if errors:
            print("Errors encountered during processing:")
            for error in errors:
                print(error)
            exit(1)

        conflicts = check_conflicts(usage)
        if conflicts:
            print("Resource conflicts detected:")
            for conflict in conflicts:
                print(conflict)
            exit(1)
        else:
            print("No conflicts detected.")
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
