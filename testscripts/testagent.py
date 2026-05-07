#!/usr/bin/python3

from udjat import agent

class SampleAgent(agent):
	def setup(self,properties):
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.value = 0
		return super().setup(properties)
	
	def refresh(self,ondemand):
		self.info("Updating values")
		print(self.value)
		return False;

def AgentFactory(properties):
	return SampleAgent(properties)
