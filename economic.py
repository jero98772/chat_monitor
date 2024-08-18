import pandas as pd
import argparse

# Function to parse the custom CSV format
def parse_row(row):
    row = row.replace('-', '').strip()
    value, data = row.split(' ', 1)
    server, user, category, subcategory = data.split(':')
    return int(value), server, user, category, subcategory

# Read and process the CSV file
def read_csv(file_path):
    # Read the file into a DataFrame
    df = pd.read_csv(file_path, header=None, names=['row'])

    # Extract the value and other data components
    df['row_clean'] = df['row'].str.replace('-', '').str.strip()
    df['value'] = df['row_clean'].str.extract(r'^(\d+)').astype(int)
    df['server'] = df['row_clean'].str.extract(r'\s(\w+):')
    df['user'] = df['row_clean'].str.extract(r':(\w+):')
    df['category'] = df['row_clean'].str.extract(r':(\w+):(\w+)$')[0]
    df['subcategory'] = df['row_clean'].str.extract(r':(\w+)$')[0]

    # Select only the relevant columns
    df = df[['value', 'server', 'user', 'category', 'subcategory']]

    return df

# Function to get the number of positive and negative incomes
def get_income_stats(df):
    positive_count = df[df['value'] > 0].shape[0]
    negative_count = df[df['value'] < 0].shape[0]
    return positive_count, negative_count

# Function to get the server with the most/least value
def get_server_stats(df):
    server_values = df.groupby('server')['value'].sum().reset_index()
    most_value_server = server_values.sort_values('value', ascending=False).iloc[0]['server']
    least_value_server = server_values.sort_values('value').iloc[0]['server']
    return most_value_server, least_value_server

# Function to get the user with the most/least value
def get_user_stats(df):
    user_values = df.groupby('user')['value'].sum().reset_index()
    most_value_user = user_values.sort_values('value', ascending=False).iloc[0]['user']
    least_value_user = user_values.sort_values('value').iloc[0]['user']
    return most_value_user, least_value_user

# Main function with argument parsing
def main():
    # Setup argument parser
    parser = argparse.ArgumentParser(description="Process a custom CSV file and output statistics.")
    parser.add_argument("file", help="Path to the CSV file")
    parser.add_argument("--income-stats", action="store_true", help="Print positive and negative income stats")
    parser.add_argument("--servers-stats", action="store_true", help="Print server stats (most and least value)")
    parser.add_argument("--users-stats", action="store_true", help="Print user stats (most and least value)")
    
    args = parser.parse_args()

    # Read the CSV file
    df = read_csv(args.file)
    
    # Print selected stats
    if args.income_stats:
        positive_incomes, negative_incomes = get_income_stats(df)
        print(f"Positive incomes: {positive_incomes}, Negative incomes: {negative_incomes}")
    
    if args.servers_stats:
        most_value_server, least_value_server = get_server_stats(df)
        print(f"Server with most value: {most_value_server}, Server with least value: {least_value_server}")
    
    if args.users_stats:
        most_value_user, least_value_user = get_user_stats(df)
        print(f"User with most value: {most_value_user}, User with least value: {least_value_user}")

# Entry point
if __name__ == "__main__":
    main()
