# Contributing to DSMR P1 Meter

Thank you for your interest in contributing to this project! Here are some guidelines to help you get started.

## How to Contribute

### Reporting Issues

If you encounter any bugs or have feature requests:

1. Check if the issue already exists in the [Issues](../../issues) section
2. If not, create a new issue with:
   - Clear description of the problem
   - Steps to reproduce (for bugs)
   - Expected vs actual behavior
   - Your hardware setup (ESP board, smart meter model)
   - ESPHome version

### Submitting Changes

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes
4. Test your changes thoroughly
5. Commit with clear, descriptive messages
6. Push to your fork
7. Submit a pull request

### ESPHome Configuration Changes

When modifying the ESPHome configuration:

- Test with real hardware when possible
- Ensure compatibility with DSMR v5 specification
- Update documentation if adding new sensors
- Consider backward compatibility

### Documentation Updates

- Keep the README.md up to date with any changes
- Update wiring diagrams if hardware changes
- Include clear explanations for new features

### Code Style

- Follow ESPHome YAML conventions
- Use consistent indentation (2 spaces)
- Add comments for complex configurations
- Keep sensor names descriptive and consistent

## Development Setup

1. Install ESPHome: `pip install esphome`
2. Clone your fork of the repository
3. Copy `ESPHome/secrets.yaml.template` to `ESPHome/secrets.yaml`
4. Configure your WiFi credentials
5. Test with: `esphome compile ESPHome/dsmr-p1-meter.yaml`

## Testing

Before submitting changes:

- Compile the configuration without errors
- Test on actual hardware if possible
- Verify all sensors are reporting correctly
- Check Home Assistant integration (if applicable)

## Questions?

Feel free to open an issue for questions about contributing or development setup.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
