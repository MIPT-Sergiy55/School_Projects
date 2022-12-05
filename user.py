import pandas as pd
import time
from uuid import uuid4


class User:
    def __init__(self, user_id: int, df_requests: pd.DataFrame, df_tasks: pd.DataFrame):
        self.user_id = user_id
        self.df_requests = df_requests
        self.df_tasks = df_tasks

    def get_task(self, task_id: int):
        return

    def get_tasks(self):
        df = self.df_tasks
        df = df.query("user_id == @self.user_id")
        df = df.filter(items=['task_name', 'task_id'])
        df = df.set_index('task_name')
        d = df.to_dict()
        d = d['task_id']
        return d

    def get_tasks_by_type(self, flag = 0):
        d = self.get_tasks()
        ans = dict()
        if len(d) == 0:
            return ans
        df = self.df_requests
        for key in [*d]:
            k = d[key]
            df_temp = df.query("user_id == @self.user_id and task_id == @k")
            if df_temp.shape[0] % 2 == flag:
                ans[key] = d[key]
        return ans

    def get_running_tasks(self):
        return self.get_tasks_by_type(1)

    def get_non_running_tasks(self):
        return self.get_tasks_by_type()

    def get_spent_time(self):
        d = self.get_non_running_tasks()
        df = self.df_requests
        ans = dict()
        for key in [*d]:
            k = d[key]
            df_temp = df.query("user_id == @self.user_id and task_id == @k")
            times = df_temp["time"].to_numpy()
            times_types = df_temp["time_type"].to_numpy()
            ans[key] = times @ times_types
        return ans

    def add_task(self, task_name):
        data = {'user_id': self.user_id, 'task_name': task_name, 'task_id': uuid4()}
        self.df_tasks.loc[self.df_tasks.shape[0]] = data

    def start_task(self, task_id: str):
        data = {'user_id': self.user_id, 'task_id': task_id,
                'time': time.time(), 'time_type': -1}
        self.df_requests.loc[self.df_requests.shape[0]] = data

    def stop_task(self, task_id: str):
        data = {'user_id': self.user_id, 'task_id': task_id,
                'time': time.time(), 'time_type': 1}
        self.df_requests.loc[self.df_requests.shape[0]] = data
