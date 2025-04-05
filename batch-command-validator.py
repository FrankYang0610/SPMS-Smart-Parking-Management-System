#!/usr/bin/env python3
"""
COMP2432 Operating Systems - Group Project
Smart Parking Management System (SPMS) Command Validator

This program validates commands from a .dat file for the SPMS system.
It checks each command for validity according to the specified rules and
outputs information about invalid commands found in the file.
"""

import re
import sys
from datetime import datetime, timedelta

valid_members = {'member_A', 'member_B', 'member_C', 'member_D', 'member_E'}

essential_mapping = {
    'battery': 'battery',
    'cable': 'cable',
    'cables': 'cable',
    'locker': 'locker',
    'lockers': 'locker',
    'umbrella': 'umbrella',
    'inflationservice': 'inflation',
    'inflation': 'inflation',
    'valetpark': 'valetpark',
    'valetparking': 'valetpark',
    'valet': 'valetpark'
}

# Define the valid time range
START_DATE = datetime(2025, 5, 10, 0, 0)
END_DATE = datetime(2025, 5, 16, 23, 59)

def get_standard_essential(essential):
    return essential_mapping.get(essential.lower(), None)

def validate_date_time_duration(date_str, time_str, duration_str):
    errors = []

    # Validate date format
    if not re.fullmatch(r'\d{4}-\d{2}-\d{2}', date_str):
        errors.append("Invalid date format")
        return errors, None, None

    # Validate time format
    if not re.fullmatch(r'^([01]\d|2[0-3]):([0-5]\d)$', time_str):
        errors.append("Invalid time format")
        return errors, None, None

    # Validate duration format
    if not re.fullmatch(r'^[1-9]\d*\.0$', duration_str):
        errors.append("Invalid duration (must be non-zero and end with .0)")
        return errors, None, None

    # Parse date and time
    try:
        year, month, day = map(int, date_str.split('-'))
        hour, minute = map(int, time_str.split(':'))
        start_datetime = datetime(year, month, day, hour, minute)

        # Parse duration
        duration_hours = int(float(duration_str))
        end_datetime = start_datetime + timedelta(hours=duration_hours)

        # Check if date and time are within the valid range
        if start_datetime < START_DATE:
            errors.append(f"Start time is before the allowed range (May 10, 2025 00:00)")

        if start_datetime > END_DATE:
            errors.append(f"Start time is after the allowed range (May 16, 2025 23:59)")

        if end_datetime > END_DATE:
            errors.append(f"End time exceeds the allowed range (May 16, 2025 23:59)")

        return errors, start_datetime, end_datetime

    except ValueError:
        errors.append("Invalid date/time values")
        return errors, None, None

def validate_add_parking(args):
    if len(args) < 4:
        return False, ["Insufficient parameters for addParking"]
    errors = []
    member_part = args[0]
    if not member_part.startswith('-member_'):
        errors.append("Invalid member format")
    else:
        member = member_part[1:]
        if member not in valid_members:
            errors.append("Invalid member name")

    date_str = args[1]
    time_str = args[2]
    duration_str = args[3]

    time_errors, start_datetime, end_datetime = validate_date_time_duration(date_str, time_str, duration_str)
    errors.extend(time_errors)

    remaining = args[4:] if len(args) > 4 else []
    p_index = 0
    if remaining and remaining[0].isdigit():
        p_index = 1
    essentials = remaining[p_index:]

    seen = set()
    for ess in essentials:
        std_ess = get_standard_essential(ess)
        if not std_ess:
            errors.append(f"Invalid essential: {ess}")
        else:
            if std_ess in seen:
                errors.append(f"Duplicate essential: {ess}")
            else:
                seen.add(std_ess)

    return len(errors) == 0, errors

def validate_add_reservation(args):
    return validate_add_parking(args)

def validate_add_event(args):
    if len(args) < 4:
        return False, ["Insufficient parameters for addEvent"]
    errors = []
    member_part = args[0]
    if not member_part.startswith('-member_'):
        errors.append("Invalid member format")
    else:
        member = member_part[1:]
        if member not in valid_members:
            errors.append("Invalid member name")

    date_str = args[1]
    time_str = args[2]
    duration_str = args[3]

    time_errors, start_datetime, end_datetime = validate_date_time_duration(date_str, time_str, duration_str)
    errors.extend(time_errors)

    essentials = args[4:] if len(args) >4 else []
    seen = set()
    for ess in essentials:
        std_ess = get_standard_essential(ess)
        if not std_ess:
            errors.append(f"Invalid essential: {ess}")
        else:
            if std_ess in seen:
                errors.append(f"Duplicate essential: {ess}")
            else:
                seen.add(std_ess)

    return len(errors) ==0, errors

def validate_book_essentials(args):
    if len(args) <4:
        return False, ["Insufficient parameters for bookEssentials"]
    errors = []
    member_part = args[0]
    if not member_part.startswith('-member_'):
        errors.append("Invalid member format")
    else:
        member = member_part[1:]
        if member not in valid_members:
            errors.append("Invalid member name")

    date_str = args[1]
    time_str = args[2]
    duration_str = args[3]

    time_errors, start_datetime, end_datetime = validate_date_time_duration(date_str, time_str, duration_str)
    errors.extend(time_errors)

    essentials = args[4:] if len(args) >4 else []
    seen = set()
    for ess in essentials:
        std_ess = get_standard_essential(ess)
        if not std_ess:
            errors.append(f"Invalid essential: {ess}")
        else:
            if std_ess in seen:
                errors.append(f"Duplicate essential: {ess}")
            else:
                seen.add(std_ess)

    return len(errors)==0, errors

def validate_import_batch(args):
    if len(args) !=1:
        return False, ["importBatch requires exactly one parameter"]
    return True, []

def validate_print_bookings(args):
    valid_algorithms = {'-fcfs', '-prio', '-opti', '-all'}
    if len(args) !=1:
        return False, ["printBookings requires exactly one parameter"]
    algo = args[0].lower()
    if algo not in valid_algorithms:
        return False, [f"Invalid algorithm: {args[0]}"]
    return True, []

def validate_end_program(args):
    if len(args) !=0:
        return False, ["endProgram should have no parameters"]
    return True, []

def validate_command(line):
    line = line.strip()
    if not line:
        return (True, [])
    parts = line.split()
    cmd = parts[0]
    args = parts[1:]
    valid_commands = {
        'addParking', 'addReservation', 'addEvent',
        'bookEssentials', 'importBatch', 'printBookings', 'endProgram'
    }
    errors = []
    if cmd not in valid_commands:
        errors.append("Invalid command type")
        return (False, errors)

    if cmd == 'addParking':
        valid, errs = validate_add_parking(args)
        errors.extend(errs)
    elif cmd == 'addReservation':
        valid, errs = validate_add_reservation(args)
        errors.extend(errs)
    elif cmd == 'addEvent':
        valid, errs = validate_add_event(args)
        errors.extend(errs)
    elif cmd == 'bookEssentials':
        valid, errs = validate_book_essentials(args)
        errors.extend(errs)
    elif cmd == 'importBatch':
        valid, errs = validate_import_batch(args)
        errors.extend(errs)
    elif cmd == 'printBookings':
        valid, errs = validate_print_bookings(args)
        errors.extend(errs)
    elif cmd == 'endProgram':
        valid, errs = validate_end_program(args)
        errors.extend(errs)

    return (len(errors) == 0, errors)

def main():
    if len(sys.argv) !=2:
        print("Usage: python validator.py <input_file.dat>")
        sys.exit(1)

    input_file = sys.argv[1]
    invalid_entries = []

    with open(input_file, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            is_valid, errors = validate_command(line)
            if not is_valid:
                invalid_entries.append( (line_num, line, errors) )

    for entry in invalid_entries:
        line_num, cmd, errs = entry
        print(f"Line {line_num}: {cmd}")
        for err in errs:
            print(f" - {err}")

    print(f"Total invalid commands: {len(invalid_entries)}")

if __name__ == '__main__':
    main()
