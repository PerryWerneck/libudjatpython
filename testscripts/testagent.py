#!/usr/bin/python3

from udjat import Agent, Action, State

class SampleAgent(Agent):
	def __init__(self,properties):
		super().__init__(properties)

		print('---------------------------------')
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.summary = "A really simple python agent"
		self.label = "Sample"
		self.value = 0

		print('value={} ({})'.format(self.value,self.summary));

		st = self.state	
		print('state: label={} summary={} icon={} level={}'.format(
			st.label,
			st.summary,
			st.icon,
			st.level
		))
	
	def refresh(self,ondemand):
		self.info("-----> Updating agent value")
		self.value = self.value + 1

		if self.value == 4:
			self.state = {
				'name': 'v3',
				'level': 'warning',
				'label': 'Sample state',
				'summary': 'The state value is 4 - Set by python code',
				'body': 'This is the body for the current state, show a more detailed info',
				'url': 'https://google.com',
			}

		if self.value == 5:
			self.state = State(
				name= 'v5',
				level = 'error',
				label = 'Sample state',
				summary = 'The state value is 5 - Set by python code',
				body= 'This is the body for the current state, show a more detailed info',
				url= 'https://google.com',
			)

		return True

class SampleAction(Action):
	def __init__(self,properties):
		super().__init__(properties)


def AgentFactory(properties):
	return SampleAgent(properties)

def ActionFactory(properties):
	return SampleAction(properties)
