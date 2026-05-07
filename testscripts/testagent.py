#!/usr/bin/python3

from udjat import logger, agent

class SampleAgent(agent):
	def setup(self,properties):
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		return super().setup(properties)

def agent_factory(properties):
	agent = SampleAgent(properties)
	agent.info("Agent factory method was called")
	return agent
