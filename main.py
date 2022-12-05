import asyncio
import time
import pandas as pd
from aiogram import Bot, Dispatcher, executor, types
from aiogram.contrib.fsm_storage.memory import MemoryStorage
from aiogram.dispatcher import FSMContext
from aiogram.dispatcher.filters.state import State, StatesGroup
from keep_alive import keep_alive
from aiogram.types import BotCommand

from user import *
keep_alive()
global API_TOKEN
global requests
global tasks


def get_token():
    f = open("token.txt", "r")
    global API_TOKEN
    API_TOKEN = f.read()


async def set_commands(bot: Bot):
    commands = [
        BotCommand(command="/start_task", description="Start task"),
        BotCommand(command="/stop_task", description="Stop running task"),
        BotCommand(command="/add_task", description="Add task by name"),
        BotCommand(command="/time_report", description="Make a report about time spent on tasks")
        # BotCommand(command="/cancel", description="Отменить текущее действие")
    ]
    await bot.set_my_commands(commands)


def register_handlers_common(dp: Dispatcher):
    dp.register_message_handler(start, commands="start", state="*")


def register_handlers_task_start(dp: Dispatcher):
    dp.register_message_handler(start_task, commands="start_task", state="*")
    dp.register_message_handler(get_start_task_id, state=SetStartTask.waiting_for_start_task_id)


def register_handlers_task_stop(dp: Dispatcher):
    dp.register_message_handler(stop_task, commands="stop_task", state="*")
    dp.register_message_handler(get_stop_task_id, state=SetStopTask.waiting_for_stop_task_id)


def register_handlers_task_add(dp: Dispatcher):
    dp.register_message_handler(add_task, commands="add_task", state="*")
    dp.register_message_handler(get_new_task_name, state=SetTaskName.waiting_for_new_task_name)


def register_handlers_time_report(dp: Dispatcher):
    dp.register_message_handler(report_time, commands="time_report", state="*")


async def report_time(message: types.Message):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    d = user.get_spent_time()
    if len(d) == 0:
        await message.answer("No finished tasks")
        return
    ans = ""
    for task in d.items():
        ans = ans + task[0] + ": " + time.strftime('%H:%M:%S', time.gmtime(task[1])) + "\n"
    await message.answer(ans)


async def add_task(message: types.Message):
    await message.answer("Name new task:")
    await SetTaskName.waiting_for_new_task_name.set()


async def get_new_task_name(message: types.Message, state: FSMContext):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    user.add_task(message.text)
    await state.finish()


async def start_task(message: types.Message):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    keyboard = types.ReplyKeyboardMarkup()
    buttons = []
    if len(user.get_non_running_tasks()) == 0:
        await message.answer("No tasks to run, please add new task", reply_markup=types.ReplyKeyboardRemove())
        return
    for task in user.get_non_running_tasks().keys():
        buttons.append(str(task))
    keyboard.add(*buttons)
    await message.answer("Choose task to start:", reply_markup=keyboard)
    await SetStartTask.waiting_for_start_task_id.set()


async def stop_task(message: types.Message):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    keyboard = types.ReplyKeyboardMarkup()
    buttons = []
    if len(user.get_running_tasks()) == 0:
        await message.answer("No running tasks", reply_markup=types.ReplyKeyboardRemove())
        return
    for task in user.get_running_tasks().keys():
        buttons.append(str(task))
    keyboard.add(*buttons)
    await message.answer("Choose task to stop:", reply_markup=keyboard)
    await SetStopTask.waiting_for_stop_task_id.set()


async def get_start_task_id(message: types.Message, state: FSMContext):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    if message.text not in user.get_non_running_tasks().keys():
        await message.answer("choose right task, please")
        return
    user.start_task(user.get_non_running_tasks()[message.text])
    await message.answer("Tasks started", reply_markup=types.ReplyKeyboardRemove())
    await state.finish()


async def get_stop_task_id(message: types.Message, state: FSMContext):
    global tasks
    global requests
    user = User(message.chat.id, requests, tasks)
    if message.text not in user.get_running_tasks().keys():
        await message.answer("choose right task, please")
        return
    user.stop_task(user.get_running_tasks()[message.text])
    await message.answer("Tasks stopped", reply_markup=types.ReplyKeyboardRemove())
    await state.finish()


async def start(message: types.Message):
    await message.answer('I am Task Manager bot')


async def echo(message: types.Message):
    await message.answer(message.text)


class SetStopTask(StatesGroup):
    waiting_for_stop_task_id = State()


class SetStartTask(StatesGroup):
    waiting_for_start_task_id = State()


class SetTaskName(StatesGroup):
    waiting_for_new_task_name = State()


async def main():
    bot = Bot(token=API_TOKEN)
    dp = Dispatcher(bot, storage=MemoryStorage())

    await set_commands(bot)

    register_handlers_common(dp)
    register_handlers_task_start(dp)
    register_handlers_task_stop(dp)
    register_handlers_task_add(dp)
    register_handlers_time_report(dp)

    await dp.start_polling()


if __name__ == '__main__':
    global requests
    global tasks
    requests = pd.read_csv('requests.csv')
    tasks = pd.read_csv('tasks_map.csv')
    get_token()
    asyncio.run(main())
